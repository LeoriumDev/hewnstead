#pragma once

#include <hewnstead/world/face.hpp>

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
constexpr BlockId Log = 3;
constexpr BlockId Planks = 4;
constexpr BlockId Grass = 5;

[[nodiscard]] std::uint32_t textureLayer(BlockId id, Face face);

}  // namespace blocks

[[nodiscard]] inline const char* blockName(BlockId id) {
    switch (id) {
    case blocks::Air:
        return "Air";
    case blocks::Stone:
        return "Stone";
    case blocks::Dirt:
        return "Dirt";
    case blocks::Log:
        return "Log";
    case blocks::Planks:
        return "Planks";
    case blocks::Grass:
        return "Grass";
    default:
        return "(null)";
    }
}

}  // namespace hs
