#include <hewnstead/world/block_id.hpp>

#include <cassert>

namespace hs::blocks {

std::uint32_t textureLayer(BlockId id, Face face) {
    switch (id) {
    case Stone: return 0;
    case Dirt: return 1;
    case Log:
        if (face == Face::Top || face == Face::Bottom) {
            return 3;  // log_top layer
        }
        return 2;  // log_side layer
    case Planks: return 4;
    case Grass:
        if (face == Face::Top) {
            return 5;  // grass_top
        }
        if (face == Face::Bottom) {
            return 1;  // dirt
        }
        return 6;  // grass_side
    case Bricks: return 7;
    case Clay: return 8;
    case CoalOre: return 9;
    case CobbleStone: return 10;
    case CopperOre: return 11;
    case Gravel: return 12;
    case IronOre: return 13;
    case Leaves: return 14;
    case MossyCobbleStone: return 15;
    case Sand: return 16;
    case Snow: return 17;
    case StoneBricks: return 18;
    case Thatch: return 19;
    default: assert(false && "textureLayer called with unknown or Air BlockId"); return 0;
    }
}
}  // namespace hs::blocks
