#include "gecsa/gecsa.hpp"

#include "CSA.h"

#include "undef_damn_macros.hpp"

namespace gecsa {

GeCSA::GeCSA() {
    //impl_ = std::make_unique<CSA>();
}

GeCSA::GeCSA(GeCSA &&other) = default;

GeCSA::~GeCSA() = default;

GeCSA &GeCSA::operator=(GeCSA &&) = default;

u32 GeCSA::block_size() const {
    return impl_->getL();
}

u32 GeCSA::sample_rate() const {
    return impl_->getD();
}

usize GeCSA::alphabet_size() const {
    return impl_->getAlphabetsize();
}

usize GeCSA::size() const {
    return impl_->getN();
}

usize GeCSA::size_in_bytes(u32 component_flags) const {
    usize result = 0;
    if (component_flags & kPhi)
        result += impl_->getPhiSizeInBytes();
    if (component_flags & kSaSamples)
        result += impl_->getSaSamplesSizeInBytes();
    if (component_flags & kIsaSamples)
        result += impl_->getIsaSamplesSizeInBytes();  
    if (component_flags & kSampleMarks)
        result += impl_->getDmapSizeInBytes();
    return result;
}

f64 GeCSA::compression_ratio(u32 component_flags) const {
    return static_cast<f64>(size_in_bytes(component_flags)) / static_cast<f64>(size());
}

f64 GeCSA::phi_block_ratio(u32 component_flags) const {
    PhiStats stats = impl_->getPhiStats();
    usize total = 0;
    if (component_flags & PhiComponent::kAll1)
        total += stats.all1;
    if (component_flags & PhiComponent::kGamma)
        total += stats.gamma;
    if (component_flags & PhiComponent::kBV)
        total += stats.bv;
    if (component_flags & PhiComponent::kRLDelta)
        total += stats.rldelta;
    return static_cast<f64>(total) / static_cast<f64>(stats.totalblocks);
}

usize GeCSA::phi_block_size(u32 component_flags) const {
    PhiStats stats = impl_->getPhiStats();
    usize total = 0;
    if (component_flags & PhiComponent::kAll1)
        total += stats.all1;
    if (component_flags & PhiComponent::kGamma)
        total += stats.gamma;
    if (component_flags & PhiComponent::kBV)
        total += stats.bv;
    if (component_flags & PhiComponent::kRLDelta)
        total += stats.rldelta;
    return total;
}

uchar8 GeCSA::map_alphabet(uchar8 unmapped) const {
    i64 mapped = impl_->LocalCharacter(unmapped);
    assert(mapped >= 0);
    return mapped;
}

uchar8 GeCSA::unmap_alphabet(uchar8 mapped) const {
    i64 unmapped = impl_->Character(mapped);
    assert(unmapped >= 0);
    return unmapped;
}

uindex GeCSA::alphabet_offset(uchar8 mapped) const {
    i64 offset = impl_->AlphabetOffset(mapped);
    assert(offset >= 0);
    return offset;
}

std::pair<uindex, uindex> GeCSA::alphabet_range(uchar8 mapped) const {
    i64 l = impl_->AlphabetOffset(mapped);
    i64 r = impl_->AlphabetOffset(mapped + 1);
    assert(l >= 0 && r >= 0);
    return {l, r};
}

uindex GeCSA::sa_sample_get(uindex index) const {
    return impl_->GetSAL()->GetValue(index);
}

uindex GeCSA::isa_sample_get(uindex index) const {
    return impl_->GetRankL()->GetValue(index);
}

bool GeCSA::sample_bitmap_get(uindex index) const {
    return impl_->GetArrD()->getD(index);
}

uindex GeCSA::sample_bitmap_rank1(uindex index) const {
    return index == 0 ? 0 : impl_->GetArrD()->getD(index - 1);
}

uindex GeCSA::sample_bitmap_rank0(uindex index) const {
    return index - sample_bitmap_rank1(index);
}

uindex GeCSA::sample_bitmap_select1(uindex index) const {
    return impl_->GetArrD()->select(index + 1);
}

usize GeCSA::count(std::span<const u8> pattern) const {
    long long result;
    impl_->counting(pattern.data(), pattern.size(), result);
    return result;
}

usize GeCSA::count(std::span<const char> pattern) const {
    long long result;
    impl_->counting(reinterpret_cast<const u8 *>(pattern.data()), pattern.size(), result);
    return result;
}

bool GeCSA::exists(std::span<const u8> pattern) const {
    return count(pattern) != 0;
}

bool GeCSA::exists(std::span<const char> pattern) const {
    return count(pattern) != 0;
}

std::pair<uindex, uindex> GeCSA::suffix_range(std::span<const u8> pattern) const {
    long long l = 0, r = 0;
    impl_->Search2(pattern.data(), pattern.size(), l, r);
    return l > r ? std::pair<uindex, uindex> {kNoIndex, kNoIndex} : std::pair<uindex, uindex> {l, r + 1};
}

std::pair<uindex, uindex> GeCSA::suffix_range(std::span<const char> pattern) const {
    return suffix_range(std::span<const u8>{reinterpret_cast<const u8 *>(pattern.data()), pattern.size()});
}

std::pair<uindex, uindex> GeCSA::suffix_range(uindex hint_l, uindex hint_r, std::span<const u8> pattern) const {
    if (pattern.empty())
        return {hint_l, hint_r};
    long long l = 0, r = 0;
    impl_->Search2(reinterpret_cast<const u8 *>(pattern.data()), pattern.size(), l, r, hint_l, hint_r - 1);
    return l > r ? std::pair<uindex, uindex> {kNoIndex, kNoIndex} : std::pair<uindex, uindex> {l, r + 1};
}

std::pair<uindex, uindex> GeCSA::suffix_range(uindex hint_l, uindex hint_r, std::span<const char> pattern) const {
    return suffix_range(hint_l, hint_r, 
        std::span<const u8>{reinterpret_cast<const u8 *>(pattern.data()), pattern.size()});
}

std::pair<uindex, uindex> GeCSA::extend_suffix_range(uindex base_l, uindex base_r, u8 c) const {
    long long l = 0, r = 0;
    impl_->SearchExtend(l, r, base_l, base_r, c);
    return l > r ? std::pair<uindex, uindex> {kNoIndex, kNoIndex} : std::pair<uindex, uindex> {l, r + 1};
}

void GeCSA::suffix_range_to_indices(std::pair<uindex, uindex> range, uindex *result_out, SortOrder order) const {
    assert(range.first != kNoIndex && range.second != kNoIndex);
    //impl->newEnumerative2(range.first, range.second - 1, reinterpret_cast<long long *>(result_out));
    for (uindex i = range.first; i < range.second; ++i)
        result_out[i - range.first] = sa(i);

    if (order == SortOrder::kAsc) {
        std::sort(result_out, result_out + range.second - range.first);
    } else if (order == SortOrder::kDesc) {
        std::sort(result_out, result_out + range.second - range.first, std::greater<>{});
    }
}

std::vector<uindex> GeCSA::suffix_range_to_indices(std::pair<uindex, uindex> range, SortOrder order) const {
    assert(range.first != kNoIndex && range.second != kNoIndex);
    std::vector<uindex> result(range.second - range.first);
    suffix_range_to_indices(range, result.data(), order);
    return result;
}

usize GeCSA::locate(std::span<const u8> pattern, uindex *result_out, SortOrder order) const {
    usize occ = impl_->locating(pattern.data(), pattern.size(), reinterpret_cast<long long *>(result_out));
    if (order == SortOrder::kAsc) {
        std::sort(result_out, result_out + occ);
    } else if (order == SortOrder::kDesc) {
        std::sort(result_out, result_out + occ, std::greater<>{});
    }
    return occ;
}

usize GeCSA::locate(std::span<const char> pattern, uindex *result_out, SortOrder order) const {
    return locate(std::span<const u8>{reinterpret_cast<const u8 *>(pattern.data()), pattern.size()},
                  result_out, order);
}

std::vector<usize> GeCSA::locate(std::span<const u8> pattern, SortOrder order) const {
    auto range = suffix_range(pattern);
    std::vector<usize> result(range.second - range.first);
    suffix_range_to_indices(range, result.data(), order);
    return result;
}

std::vector<usize> GeCSA::locate(std::span<const char> pattern, SortOrder order) const {
    auto range = suffix_range(pattern);
    std::vector<usize> result(range.second - range.first);
    suffix_range_to_indices(range, result.data(), order);
    return result;
}

uindex GeCSA::sa(uindex index) const {
    return impl_->GetPos(index);
}

void GeCSA::extract(uindex start, usize len, u8 *result_out) const {
    impl_->extracting(start, len, result_out);
}

void GeCSA::extract(uindex start, usize len, char *result_out) const {
    extract(start, len, reinterpret_cast<u8 *>(result_out));
}

std::vector<u8> GeCSA::extract(uindex start, usize len) const {
    std::vector<u8> result(len);
    extract(start, len, result.data());
    return result;
}

void GeCSA::save(const std::string &path) const {
    impl_->save(path.c_str());
}

void GeCSA::save(FILE *file) const {
    impl_->save(file);
}

void GeCSA::swap(GeCSA &other) {
    impl_.swap(other.impl_);
}

GeCSA GeCSA::create_from_file(const std::string &path, u32 block_size, u32 sample_rate) {
    GeCSA csa;
    csa.impl_ = std::make_unique<CSA>(path.c_str(), block_size, sample_rate);
    return std::move(csa);
}

GeCSA GeCSA::create_from_str(std::span<const u8> buf, u32 block_size, u32 sample_rate) {
    GeCSA csa;
    csa.impl_ = std::make_unique<CSA>(buf.data(), buf.size(), block_size, sample_rate);
    return std::move(csa);
}

GeCSA GeCSA::create_from_str(std::span<const char> buf, u32 block_size, u32 sample_rate) {
    GeCSA csa;
    csa.impl_ = std::make_unique<CSA>(reinterpret_cast<const u8 *>(buf.data()), buf.size(), block_size, sample_rate);
    return std::move(csa);
}

GeCSA GeCSA::load_from_file(const std::string &path) {
    GeCSA csa;
    csa.impl_ = std::make_unique<CSA>();
    csa.impl_->load(path.c_str());
    return std::move(csa);
}

GeCSA GeCSA::load_from_file(FILE *file) {
    GeCSA csa;
    csa.impl_ = std::make_unique<CSA>();
    csa.impl_->load(file);
    return std::move(csa);
}

}