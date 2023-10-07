#include "gecsa/sparse_bitmap.hpp"

#include <algorithm>
#include <bit>
#include <cassert>
#include <vector>

#include "InArray.h"
#include "undef_damn_macros.hpp"

namespace gecsa {

SparseBitMap::SparseBitMap() = default;

SparseBitMap::SparseBitMap(usize len, std::span<uindex> one_positions, u32 block_size, u32 superblock_rate)
    : block_size_(block_size), superblock_rate_(superblock_rate) {
    impl_ = std::make_unique<InArray>();
    impl_->constructrank(len, reinterpret_cast<long long unsigned int *>(one_positions.data()), one_positions.size(), block_size, superblock_rate);
}

SparseBitMap::SparseBitMap(const BitMap &bitmap, u32 block_size, u32 superblock_rate)
    : block_size_(block_size), superblock_rate_(superblock_rate) {
    std::vector<uindex> one_positions;
    for (uindex i = 0; i < bitmap.size(); ++i) {
        if (bitmap[i]) {
            one_positions.push_back(i);
        }
    }
    
    impl_ = std::make_unique<InArray>();
    impl_->constructrank(bitmap.size(), reinterpret_cast<long long unsigned int *>(one_positions.data()), one_positions.size(), block_size, superblock_rate);
}

SparseBitMap::SparseBitMap(const SparseBitMap &other)
    : block_size_(other.block_size()), superblock_rate_(other.superblock_rate()) {
    impl_ = std::make_unique<InArray>(other.impl_->GetNum(), other.impl_->GetDataWidth());
    for (uindex i = 0; i < other.impl_->GetNum(); ++i) {
        impl_->SetValue(i, other.impl_->GetValue(i));
    }
    impl_->constructrank(256, 16);
}

SparseBitMap::SparseBitMap(SparseBitMap &&other) = default;

SparseBitMap::~SparseBitMap() = default;

SparseBitMap &SparseBitMap::operator=(const SparseBitMap &other) {
    SparseBitMap cpy(other);
    swap(cpy);
    return *this;
}

SparseBitMap &SparseBitMap::operator=(SparseBitMap &&other) = default;

usize SparseBitMap::size_in_bytes() const {
    return impl_->GetrankMemorySize();
}

u32 SparseBitMap::block_size() const {
    return block_size_;
}

u32 SparseBitMap::superblock_rate() const {
    return superblock_rate_;
}

usize SparseBitMap::size() const {
    return impl_->GetNum();
}

bool SparseBitMap::empty() const {
    return size() == 0;
}

bool SparseBitMap::at(uindex index) const {
    return impl_->getD(index);
}

bool SparseBitMap::operator[](uindex index) const {
    return at(index);
}

usize SparseBitMap::rank1(uindex index) const {
    return index == 0 ? 0 : impl_->rankGeneral(index - 1);
}

usize SparseBitMap::rank1(uindex first, uindex last) const {
    return rank1(last) - rank1(first);
}

usize SparseBitMap::rank0(uindex index) const {
    return index - rank1(index);
}

usize SparseBitMap::rank0(uindex first, uindex last) const {
    return rank0(last) - rank1(first);
}

usize SparseBitMap::select1(uindex index) const {
    return impl_->select(index + 1);
}

void SparseBitMap::save(const std::string &path) const {
    savekit kit(path.c_str());
    impl_->write(kit);
}

void SparseBitMap::save(FILE *file) const {
    savekit kit;
    kit.setfile(file);
    impl_->write(kit);
    kit.setfile();
}

void SparseBitMap::swap(SparseBitMap &other) {
    impl_.swap(other.impl_);
    std::swap(block_size_, other.block_size_);
    std::swap(superblock_rate_, other.superblock_rate_);
}

SparseBitMap SparseBitMap::load_from_file(const std::string &path) {
    SparseBitMap bitmap;
    bitmap.impl_ = std::make_unique<InArray>();
    loadkit kit(path.c_str());
    bitmap.impl_->load(kit, true);
    return std::move(bitmap);
}

SparseBitMap SparseBitMap::load_from_file(FILE *file) {
    SparseBitMap bitmap;
    bitmap.impl_ = std::make_unique<InArray>();
    loadkit kit;
    kit.setfile(file);
    bitmap.impl_->load(kit, true);
    kit.setfile();
    return std::move(bitmap);
}

}