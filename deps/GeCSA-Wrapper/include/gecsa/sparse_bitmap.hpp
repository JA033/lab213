#pragma once

#include <memory>
#include <span>

#include "bitmap.hpp"
#include "int_array.hpp"
#include "int_types.hpp"

class InArray;

namespace gecsa {

class SparseBitMap {
public:
    SparseBitMap();
    SparseBitMap(usize len, std::span<uindex> one_positions, u32 block_size = 256, u32 superblock_rate = 16);
    SparseBitMap(const BitMap &bitmap, u32 block_size = 256, u32 superblock_rate = 16);

    SparseBitMap(const SparseBitMap &other);
    SparseBitMap(SparseBitMap &&other);
    ~SparseBitMap();

    SparseBitMap &operator=(const SparseBitMap &other);
    SparseBitMap &operator=(SparseBitMap &&other);

    [[nodiscard]] usize size_in_bytes() const;
    [[nodiscard]] u32 block_size() const;
    [[nodiscard]] u32 superblock_rate() const;

    [[nodiscard]] usize size() const;
    [[nodiscard]] bool empty() const;
    [[nodiscard]] bool at(uindex index) const;
    [[nodiscard]] bool operator[](uindex index) const;
    [[nodiscard]] usize rank1(uindex index) const;
    [[nodiscard]] usize rank1(uindex first, uindex last) const;
    [[nodiscard]] usize rank0(uindex index) const;
    [[nodiscard]] usize rank0(uindex first, uindex last) const;
    [[nodiscard]] usize select1(uindex index) const;

    void save(const std::string &path) const;
    void save(FILE *file) const;

    void swap(SparseBitMap &other);

    [[nodiscard]] static SparseBitMap load_from_file(const std::string &path);
    [[nodiscard]] static SparseBitMap load_from_file(FILE *file);

private:
    std::unique_ptr<InArray> impl_;
    u32                      block_size_;
    u32                      superblock_rate_;
};

}