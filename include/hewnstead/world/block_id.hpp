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
constexpr BlockId Bricks = 6;
constexpr BlockId Clay = 7;
constexpr BlockId CoalOre = 8;
constexpr BlockId CobbleStone = 9;
constexpr BlockId CopperOre = 10;
constexpr BlockId Gravel = 11;
constexpr BlockId IronOre = 12;
constexpr BlockId Leaves = 13;
constexpr BlockId MossyCobbleStone = 14;
constexpr BlockId Sand = 15;
constexpr BlockId Snow = 16;
constexpr BlockId StoneBricks = 17;
constexpr BlockId Thatch = 18;

[[nodiscard]] std::uint32_t textureLayer(BlockId id, Face face);

}  // namespace blocks

[[nodiscard]] inline const char* blockName(BlockId id) {
    switch (id) {
    case blocks::Air: return "Air";
    case blocks::Stone: return "Stone";
    case blocks::Dirt: return "Dirt";
    case blocks::Log: return "Log";
    case blocks::Planks: return "Planks";
    case blocks::Grass: return "Grass";
    case blocks::Bricks: return "Bricks";
    case blocks::Clay: return "Clay";
    case blocks::CoalOre: return "CoalOre";
    case blocks::CobbleStone: return "CobbleStone";
    case blocks::CopperOre: return "CopperOre";
    case blocks::Gravel: return "Gravel";
    case blocks::IronOre: return "IronOre";
    case blocks::Leaves: return "Leaves";
    case blocks::MossyCobbleStone: return "MossyCobbleStone";
    case blocks::Sand: return "Sand";
    case blocks::Snow: return "Snow";
    case blocks::StoneBricks: return "StoneBricks";
    case blocks::Thatch: return "Thatch";
    default: return "(unknown)";
    }
}

}  // namespace hs
