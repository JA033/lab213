#pragma once

#include "gecsa/bitmap.hpp"
#include "gecsa/int_array.hpp"

#include "typedefs.hpp"

namespace wildcard {

class SearchableBitmap {
public:
    SearchableBitmap();
    SearchableBitmap(const SearchableBitmap &other);
    SearchableBitmap(SearchableBitmap &&other);

    SearchableBitmap(gecsa::BitMap &&bitmap, bool no_select1_support = false);
    SearchableBitmap(const gecsa::BitMap &bitmap, bool no_select1_support = false);

    SearchableBitmap &operator=(const SearchableBitmap &other);
    SearchableBitmap &operator=(SearchableBitmap &&other);

    [[nodiscard]] usize size_in_bytes() const;
    [[nodiscard]] usize size() const { return bitmap_.size(); }
    [[nodiscard]] bool empty() const { return size() == 0; }
    [[nodiscard]] bool operator[](uindex index) const { return bitmap_[index]; }

    [[nodiscard]] bool support_rank() const { return true; }
    [[nodiscard]] bool support_select1() const { return support_select1_; }
    [[nodiscard]] usize rank1(uindex index) const;
    [[nodiscard]] usize rank0(uindex index) const;
    [[nodiscard]] usize rank1(uindex first, uindex last) const;
    [[nodiscard]] usize rank0(uindex first, uindex last) const;
    [[nodiscard]] usize select1(uindex index) const;

    void swap(SearchableBitmap &other);

private:
    bool             support_select1_;
    gecsa::BitMap    bitmap_;
    std::vector<u64> upperblocks_;
    std::vector<u64> block_entries_;
    std::vector<u32> sampled_positions_;

    void ConstructRankSupport();
    void ConstructSelect1Support();
};

}