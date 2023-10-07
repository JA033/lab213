#include "utils.hpp"

#include <bit>
#include <cassert>
#include <cstring>
#include <numeric>

#include <immintrin.h>

namespace wildcard {

ByteSequence join_strs(std::vector<ConstByteSpan> strs, ubyte8 delimiter, bool prepend, bool append, 
                       std::vector<uindex> *delimiter_pos) {
    if (strs.empty())
        return {};
    
    usize size_in_bytes = strs.size() - 1 + static_cast<usize>(prepend) + static_cast<usize>(append);
    if (delimiter_pos) {
        delimiter_pos->resize(size_in_bytes);
    }

    size_in_bytes = std::transform_reduce(strs.begin(), strs.end(), size_in_bytes, std::plus<>{}, 
        [](ConstByteSpan str) { return str.size(); }
    );
    
    ByteSequence result(size_in_bytes);
    uindex writer_index = 0;

    auto write_delimiter = [&](ubyte8 d) {
        result[writer_index++] = d;
    };

    auto write_str = [&](ConstByteSpan str) {
        memcpy(result.data() + writer_index, str.data(), str.size());
        writer_index += str.size();
    };

    if (prepend) {
        if (delimiter_pos) {
            delimiter_pos->front() = writer_index;
        }
        write_delimiter(delimiter);
    }
    for (uindex i = 0; i < strs.size() - 1; ++i) {
        write_str(strs[i]);
        if (delimiter_pos) {
            (*delimiter_pos)[i + 1] = writer_index;
        }
        write_delimiter(delimiter);
    }
    write_str(strs.back());
    if (append) {
        if (delimiter_pos) {
            delimiter_pos->back() = writer_index;
        }
        write_delimiter(delimiter);
    }
    return result;
}

void mem_prefetch(const void *ptr) {
    __builtin_prefetch(ptr);
}

u32 select1(u32 block, u32 k) {
    assert(k < 32);
    return std::countr_zero(_pdep_u32(static_cast<u32>(1) << k, block));
}

u32 select1(u64 block, u32 k) {
    assert(k < 64);
    return std::countr_zero(_pdep_u64(static_cast<u64>(1) << k, block));
}

}