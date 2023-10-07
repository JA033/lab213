#include "searchable_bitmap.hpp"

#include <cassert>
#include <limits>
#include <immintrin.h>

#include "utils.hpp"

namespace wildcard {

constexpr usize kRawblockSize = 32;

constexpr usize kBlockSize = 512;
constexpr usize kSuperblockSize = kBlockSize * 4;
constexpr usize kUpperblockSize = 1U << 31;

constexpr usize kRawblocksPerBlock = kBlockSize / kRawblockSize;
constexpr usize kRawblocksPerSuperblock = kSuperblockSize / kRawblockSize;

constexpr usize kSuperblocksPerUpperblock = kUpperblockSize / kSuperblockSize;
constexpr usize kBlocksPerSuperblock = kSuperblockSize / kBlockSize;

constexpr usize kSampledPositionsSize = 8192;
constexpr usize kSampledPositionsPerUpperblock = kUpperblockSize / kSampledPositionsSize;

struct BlockEntry {
    u32 l1, l2;
    
    BlockEntry(u32 l1, u32 l2_1, u32 l2_2, u32 l2_3)
        : l1(l1), l2((l2_3 << 20) | (l2_2 << 10) | l2_1) { }
    
    [[nodiscard]] u32 GetL2(u32 index) const {
        return (l2 >>  ((index - 1) * 10)) & 0x3FFU;
    }
};

static bool BlockEntryL1LessThan(u64 a, u64 b) {
    return std::bit_cast<BlockEntry>(a).l1 <
           std::bit_cast<BlockEntry>(b).l1;
}

SearchableBitmap::SearchableBitmap(gecsa::BitMap &&bitmap, bool no_select1_support)
    : bitmap_(std::move(bitmap)),
      support_select1_(!no_select1_support) {
    ConstructRankSupport();
    if (support_select1())
        ConstructSelect1Support();
}

SearchableBitmap::SearchableBitmap(const gecsa::BitMap &bitmap, bool no_select1_support)
    : bitmap_(bitmap),
      support_select1_(!no_select1_support) {
    ConstructRankSupport();
    if (support_select1())
        ConstructSelect1Support();
}

usize SearchableBitmap::size_in_bytes() const {
    return bitmap_.size_in_bytes() +
           upperblocks_.size() * sizeof(u64) +
           block_entries_.size() * sizeof(u64) +
           sampled_positions_.size() * sizeof(u32);
}

usize SearchableBitmap::rank1(uindex index) const {
    assert(index <= size());
    
    if (index == 0) [[unlikely]]
        return 0;
    
    const u32 *raw = bitmap_.raw();
    uindex upperblock_idx = index >> 31;
    uindex block_idx = index / kBlockSize;

    auto [superblock_idx, superblock_rem] = idiv(block_idx, kBlocksPerSuperblock);

    BlockEntry entry = std::bit_cast<BlockEntry>(block_entries_[superblock_idx]);
    usize result = upperblocks_[upperblock_idx] + entry.l1;

    static const u32 kL2Masks[4] {0x0U, 0x3FFU, 0xFFFFFU, 0x3FFFFFFFU};
    __m64 l2 = (__m64) _pdep_u64(entry.l2 & kL2Masks[superblock_rem], 0x03FF03FF03FF);
    l2 = _mm_hadd_pi16(l2, l2);
    l2 = _mm_hadd_pi16(l2, l2);
    result += (u64)(l2) & 0xFFFFULL;

    u32 uncounted = index % kBlockSize;
    const u32 uncounted_raw = uncounted / kRawblockSize;
    const usize base = (index - uncounted) / kRawblockSize;

    mem_prefetch(raw + base);
    for (u32 i = 0; i < uncounted_raw; ++i)
        result += std::popcount(raw[base + i]);
    uncounted %= 32;

    if (uncounted != 0) [[unlikely]] {
        u32 block = raw[base + uncounted_raw];
        block >>= 32 - uncounted;
        result += std::popcount(block);
    }
    return result;
}

usize SearchableBitmap::rank0(uindex index) const {
    return index - rank1(index);
}

usize SearchableBitmap::rank1(uindex first, uindex last) const {
    return rank1(last) - rank1(first);
}

usize SearchableBitmap::rank0(uindex first, uindex last) const {
    return rank0(last) - rank0(first);
}

usize SearchableBitmap::select1(uindex index) const {
    assert(support_select1());
    assert(index < size());

    uindex upperblock_idx = std::distance(upperblocks_.begin(),
        upper_bound(std::ranges::subrange(upperblocks_.begin(), upperblocks_.end() - 1), index)) - 1;

    const uindex upper_block = upperblocks_[upperblock_idx];

    auto [sampled_idx, unchecked] = idiv(index + 1, kSampledPositionsSize);
    uindex pos = upper_block + sampled_positions_[sampled_idx];

    uindex superblock_idx = (pos + unchecked) / kSuperblockSize;
    auto iter = upper_bound(std::ranges::subrange(block_entries_.begin() + superblock_idx,
                            block_entries_.end() - 1), 
                            index - upperblock_idx * kUpperblockSize,
                            &BlockEntryL1LessThan);
    superblock_idx = std::distance(block_entries_.begin(), iter) - 1;

    BlockEntry entry = std::bit_cast<BlockEntry>(block_entries_[superblock_idx]);
    pos = superblock_idx * kSuperblockSize;
    unchecked = index - upper_block - entry.l1 + 1;

    u32 block_idx = 1;
    while (block_idx <= 3) {
        u32 l2 = entry.GetL2(block_idx);
        if (l2 < unchecked) {
            unchecked -= l2;
            pos += kBlockSize;
            ++block_idx;
        } else { 
            break;
        }
    }

    const u32 base = superblock_idx * kBlocksPerSuperblock * kRawblocksPerBlock +
                    (block_idx - 1) * kRawblocksPerBlock;
    u64 block;
    for (u32 i = 0; i < kRawblocksPerBlock; ++i) {
        block = bitmap_.raw()[base + i];
        u32 popcnt = std::popcount(block);
        if (popcnt < unchecked) {
            unchecked -= popcnt;
            pos += kBlockSize;
        } else { 
            break;
        }
    }

    if (unchecked != 0) [[unlikely]] {
        pos += ::wildcard::select1(block, unchecked - 1);
    }

    return pos;
}

void SearchableBitmap::swap(SearchableBitmap &other) {
    bitmap_.swap(other.bitmap_);
    upperblocks_.swap(other.upperblocks_);
    block_entries_.swap(other.block_entries_);
    sampled_positions_.swap(other.sampled_positions_);
}

void SearchableBitmap::ConstructRankSupport() {
    const usize n_rawblocks = div_ceil(bitmap_.size(), kRawblockSize);
    const usize n_blocks = div_ceil(bitmap_.size(), kBlockSize);
    const usize n_superblocks = div_ceil(n_blocks, kBlocksPerSuperblock) + 1;
    const usize n_upperblocks = div_ceil(bitmap_.size(), kUpperblockSize) + 1;

    upperblocks_.resize(n_upperblocks, 0);
    block_entries_.resize(n_superblocks);

    const auto [n_full_block, n_rem_block] = idiv(n_rawblocks, kRawblocksPerBlock);
    const auto [n_full_superblock, n_rem_superblock] = idiv(n_full_block, kBlocksPerSuperblock);

    const u32 *raw = bitmap_.raw();

    u32 popcount_acc = 0;

    auto block_popcount = [](const u32 *blocks) {
        u32 result = 0;
        for (uindex i = 0; i < 16; ++i)
            result += std::popcount(blocks[i]);
        return result;
    };

    for (uindex i = 0; i < n_full_superblock; ++i) {
        if (i % kSuperblocksPerUpperblock == 0) [[unlikely]] {
            const uindex upperblock_idx = i / kSuperblocksPerUpperblock;
            const uindex prev_upperblock = upperblocks_[
                std::max(upperblock_idx, static_cast<uindex>(1))
            ];
            popcount_acc = 0;
        }

        const uindex base = i * kRawblocksPerSuperblock;
        u32 p0 = block_popcount(raw + base);
        u32 p1 = block_popcount(raw + base + 16);
        u32 p2 = block_popcount(raw + base + 32);
        u32 p3 = block_popcount(raw + base + 48);

        BlockEntry entry(popcount_acc, p0, p1, p2);
        block_entries_[i] = std::bit_cast<u64>(entry);
        popcount_acc += p0 + p1 + p2 + p3;
    }

    if (n_full_superblock % kSuperblocksPerUpperblock == 0) [[unlikely]] {
        const uindex upperblock_idx = n_full_superblock / kSuperblocksPerUpperblock;
        const uindex prev_upperblock = upperblocks_[
            std::max(upperblock_idx, static_cast<uindex>(1)) - 1
        ];
        upperblocks_[upperblock_idx] = prev_upperblock + popcount_acc;
        popcount_acc = 0;
    }

    std::array<u32, 4> l2s {0, 0, 0, 0};
    uindex base = n_full_superblock * kRawblocksPerSuperblock;

    for (uindex i = 0; i < n_rem_superblock; ++i) {
        const uindex rawblock_idx = base + i * kRawblocksPerBlock;
        u32 p = block_popcount(raw + rawblock_idx);
        l2s[i] = p;
    }

    base += n_rem_superblock * kRawblocksPerBlock;
    for (uindex i = 0; i < n_rem_block; ++i) {
        const u32 block = raw[base + i];
        l2s[n_rem_superblock] += std::popcount(block);
    }

    BlockEntry entry(popcount_acc, l2s[0], l2s[1], l2s[2]);
    block_entries_[n_full_superblock] = std::bit_cast<u64>(entry);
}

void SearchableBitmap::ConstructSelect1Support() {
    usize n_sampled_positions = div_ceil(rank1(size()), kSampledPositionsSize);
    sampled_positions_.resize(n_sampled_positions);

    uindex search_begin = 0;
    sampled_positions_[0] = 0;

    for (uindex i = 1; i < sampled_positions_.size(); ++i) {
        const uindex kth = i * kSampledPositionsSize;
        auto view = std::views::iota(search_begin, size());
        auto iter = lower_bound(view, kth, {}, [this](uindex idx) {
            return rank1(idx);
        });
        if (iter != std::ranges::end(view)) {
            uindex pos = std::distance(std::ranges::begin(view), iter);
            uindex local_pos = pos % kUpperblockSize;
            sampled_positions_[i] = local_pos;
            search_begin = pos;
        }
    }
}

}
