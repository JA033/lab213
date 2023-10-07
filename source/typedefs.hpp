#pragma once

#include <span>
#include <vector>

#include "gecsa/int_types.hpp"

namespace wildcard {

using namespace gecsa::types;
using gecsa::SortOrder;

using ByteSequence = std::vector<u8>;
using ByteSpan = std::span<u8>;
using ConstByteSpan = std::span<const u8>;

}