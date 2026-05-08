#pragma once

#include <cstdint>
#include <type_traits>

namespace hs {

using BlockId = std::uint16_t;

static_assert(sizeof(BlockId) == 2,
              "BlockId is serialized as 2 bytes; size change breaks region files");
static_assert(std::is_unsigned_v<BlockId>, "BlockId is treated as unsigned in palette compression");

namespace blocks {

constexpr BlockId Air = 0;
constexpr BlockId Stone = 1;
constexpr BlockId Dirt = 2;

}  // namespace blocks

}  // namespace hs
