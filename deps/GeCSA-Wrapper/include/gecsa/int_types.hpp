#pragma once

#include <cstdint>
#include <cstdlib>

namespace gecsa {

namespace types {

using i8        = int8_t;
using i16       = int16_t;
using i32       = int32_t;
using i64       = int64_t;
using ssize32   = i32;
using ssize64   = i64;
using sindex32  = i32;
using sindex64  = i64;

using u8        = uint8_t;
using u16       = uint16_t;
using u32       = uint32_t;
using u64       = uint64_t;
using usize32   = u32;
using usize64   = u64;
using uindex32  = u32;
using uindex64  = u64;

using f32       = float;
using f64       = double;

using uchar8    = unsigned char;
using schar8    = signed char;
using ubyte8    = unsigned char;
using sbyte8    = signed char;

using wchar16   = wchar_t;
using char16    = char16_t;
using char32    = char32_t;

using sindex    = ssize64;
using ssize     = ssize64;
using soffset   = ssize64;
using sptrdiff  = ssize64;

using uindex    = usize64;
using usize     = usize64;
using uoffset   = usize64;
using uptrdiff  = usize64;
using uintptr   = usize64;

inline constexpr uindex kNoIndex = -1;

}

using namespace types;

enum class SortOrder : u32 {
    kNone,
    kDesc,
    kAsc
};

}