#pragma once

#include <memory>
#include <vector>
#include <span>

#include "int_array.hpp"
#include "sparse_bitmap.hpp"

class CSA;

namespace gecsa {

struct PhiComponent {
    enum : u32 {
        kAll1 = 1U << 0, kGamma = 1U << 1, kBV = 1U << 2, kRLDelta = 1U << 3,
        kAll = kAll1 | kGamma | kBV | kRLDelta
    };
};

class GeCSA {
public:
    enum ComponentBit : u32 {
        kPhi = 1U << 0, kSaSamples = 1U << 1, kIsaSamples = 1U << 2, kSampleMarks = 1U << 3,
        kSamples = kSaSamples | kIsaSamples,
        kSamplesWithMarks = kSamples | kSampleMarks,
        kAll = kPhi | kSamplesWithMarks
    };

public:
    GeCSA();
    GeCSA(const GeCSA &) = delete;
    GeCSA(GeCSA &&other);

    ~GeCSA();

    GeCSA &operator=(const GeCSA &) = delete;
    GeCSA &operator=(GeCSA &&);

    [[nodiscard]] u32 block_size() const;
    [[nodiscard]] u32 sample_rate() const;
    [[nodiscard]] usize alphabet_size() const;
    [[nodiscard]] usize size() const;
    [[nodiscard]] usize size_in_bytes(u32 component_flags = kAll) const;
    [[nodiscard]] f64 compression_ratio(u32 component_flags = kAll) const;
    
    [[nodiscard]] f64 phi_block_ratio(u32 component_flags) const;
    [[nodiscard]] usize phi_block_size(u32 component_flags) const;

    [[nodiscard]] uchar8 map_alphabet(uchar8 unmapped) const;
    [[nodiscard]] uchar8 unmap_alphabet(uchar8 mapped) const;
    [[nodiscard]] uindex alphabet_offset(uchar8 mapped) const;
    [[nodiscard]] std::pair<uindex, uindex> alphabet_range(uchar8 mapped) const;

    [[nodiscard]] uindex sa_sample_get(uindex index) const;
    [[nodiscard]] uindex isa_sample_get(uindex index) const;
    [[nodiscard]] bool sample_bitmap_get(uindex index) const;
    [[nodiscard]] uindex sample_bitmap_rank1(uindex index) const;
    [[nodiscard]] uindex sample_bitmap_rank0(uindex index) const;
    [[nodiscard]] uindex sample_bitmap_select1(uindex index) const;

    [[nodiscard]] usize count(std::span<const u8> pattern) const;
    [[nodiscard]] usize count(std::span<const char> pattern) const;
    [[nodiscard]] bool exists(std::span<const u8> pattern) const;
    [[nodiscard]] bool exists(std::span<const char> pattern) const;
    [[nodiscard]] std::pair<uindex, uindex> suffix_range(std::span<const u8> pattern) const;
    [[nodiscard]] std::pair<uindex, uindex> suffix_range(std::span<const char> pattern) const;
    [[nodiscard]] std::pair<uindex, uindex> suffix_range(uindex hint_l, uindex hint_r, std::span<const u8> pattern) const;
    [[nodiscard]] std::pair<uindex, uindex> suffix_range(uindex hint_l, uindex hint_r, std::span<const char> pattern) const;
    [[nodiscard]] std::pair<uindex, uindex> extend_suffix_range(uindex base_l, uindex base_r, u8 c) const;
    void suffix_range_to_indices(std::pair<uindex, uindex> range, uindex *result_out, SortOrder order = SortOrder::kNone) const;
    [[nodiscard]] std::vector<uindex> suffix_range_to_indices(std::pair<uindex, uindex> range, SortOrder order = SortOrder::kNone) const;
    [[nodiscard]] usize locate(std::span<const u8> pattern, uindex *result_out, SortOrder order = SortOrder::kNone) const;
    [[nodiscard]] usize locate(std::span<const char> pattern, uindex *result_out, SortOrder order = SortOrder::kNone) const;
    [[nodiscard]] std::vector<uindex> locate(std::span<const u8> pattern, SortOrder order = SortOrder::kNone) const;
    [[nodiscard]] std::vector<uindex> locate(std::span<const char> pattern, SortOrder order = SortOrder::kNone) const;

    [[nodiscard]] uindex sa(uindex index) const;

    void extract(uindex start, usize len, u8 *result_out) const;
    void extract(uindex start, usize len, char *result_out) const;
    [[nodiscard]] std::vector<u8> extract(uindex start, usize len) const;

    void save(const std::string &path) const;
    void save(FILE *file) const;

    void swap(GeCSA &other);

    [[nodiscard]] static GeCSA create_from_file(const std::string &path, u32 block_size = 256, u32 sample_rate = 256);
    [[nodiscard]] static GeCSA create_from_str(std::span<const u8> buf, u32 block_size = 256, u32 sample_rate = 256);
    [[nodiscard]] static GeCSA create_from_str(std::span<const char> buf, u32 block_size = 256, u32 sample_rate = 256);

    [[nodiscard]] static GeCSA load_from_file(const std::string &path);
    [[nodiscard]] static GeCSA load_from_file(FILE *file);

private:
    std::unique_ptr<CSA> impl_;
};

}