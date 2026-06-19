#include <hewnstead/world/block_id.hpp>
#include <hewnstead/world/chunk_manager.hpp>
#include <hewnstead/world/worldgen.hpp>

namespace hs::worldgen {

namespace {

constexpr float FREQ = 0.7F;
constexpr int BASE_HEIGHT = 32;
constexpr int AMPLITUDE = 64;
constexpr int SEED = 2053;

constexpr int DIRT_DEPTH = 5;
constexpr int SNOW_DEPTH = 3;
constexpr int STONE_DEPTH = 5;

constexpr float SNOW_JITTER_FREQ = 3.0F;
constexpr int SNOW_JITTER = 6;
constexpr int SNOW_JITTER_SEED = 2094;

constexpr float STONE_JITTER_FREQ = 4.0F;
constexpr int STONE_JITTER = 6;
constexpr int STONE_JITTER_SEED = 3456;

constexpr float GRASS_JITTER_FREQ = 2.0F;
constexpr int GRASS_JITTER = 4;
constexpr int GRASS_JITTER_SEED = 9012;

constexpr int CELL = 8;      //  spacing between trees
constexpr int CANOPY_R = 2;  // canopy radius

constexpr std::uint32_t hashCell(int cx, int cz, std::uint32_t salt) {
    auto h = (static_cast<std::uint32_t>(cx) * 0x9E3779B1u) ^
             (static_cast<std::uint32_t>(cz) * 0x85EBCA77u) ^ salt;
    h ^= h >> 15;
    h *= 0xC2B2AE3Du;
    h ^= h >> 13;
    return h;
}

constexpr BlockId blockForDepth(int depth, int wy, int snowLine, int stoneLine, int grassLine) {
    if (depth == 0) {
        return wy > snowLine      ? blocks::Snow
               : (wy > stoneLine) ? blocks::Stone
               : (wy > grassLine) ? blocks::Grass
                                  : blocks::Sand;
    }
    if (wy > snowLine && depth <= SNOW_DEPTH) {
        return blocks::Snow;
    }
    if (wy > stoneLine && depth <= STONE_DEPTH) {
        return blocks::Stone;
    }
    if (depth <= DIRT_DEPTH) {
        return (wy > grassLine) ? blocks::Dirt : blocks::Sand;
    }
    return blocks::Stone;
}

int surfaceHeight(int wx, int wz, const FastNoise::SmartNode<>& gen) {
    float n = gen->GenSingle2D(static_cast<float>(wx) * FREQ, static_cast<float>(wz) * FREQ, SEED);
    return BASE_HEIGHT + static_cast<int>(n * AMPLITUDE);
}

void stampTree(Chunk& chunk, ChunkCoord coord, int bx, int by, int bz, std::uint32_t h) {
    auto place = [&](int wx, int wy, int wz, BlockId id) {
        if (ChunkManager::worldToChunk(wx, wy, wz) != coord) {
            return;
        }
        chunk.set(ChunkManager::worldToLocal(wx, wy, wz), id);
    };
    int trunkH = 4 + static_cast<int>((h >> 9) & 3);
    int top = by + trunkH;
    for (int dy = 0; dy < 2; ++dy) {
        for (int dx = -CANOPY_R; dx <= CANOPY_R; ++dx) {
            for (int dz = -CANOPY_R; dz <= CANOPY_R; ++dz) {
                bool corner =
                    (dx == CANOPY_R || dx == -CANOPY_R) && (dz == CANOPY_R || dz == -CANOPY_R);
                if (corner) {
                    continue;
                }
                place(bx + dx, top - 2 + dy, bz + dz, blocks::Leaves);
            }
        }
    }
    for (int dy = 0; dy < 2; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            for (int dz = -1; dz <= 1; ++dz) {
                place(bx + dx, top + dy, bz + dz, blocks::Leaves);
            }
        }
    }
    for (int wy = by; wy < top; ++wy) {
        place(bx, wy, bz, blocks::Log);
    }
}

constexpr int floorDiv(int a, int b) {
    return (a >= 0 ? a : a - b + 1) / b;
}

}  // namespace

void generateChunkTerrain(Chunk& chunk, ChunkCoord coord, const FastNoise::SmartNode<>& generator) {
    glm::ivec3 worldCoord = ChunkManager::chunkToWorld(coord);
    auto surfaceBlockTop = [&](int wx, int wz) -> BlockId {
        int h = surfaceHeight(wx, wz, generator);
        auto line = [&](float f, int seed, int base, int amp) {
            float v = generator->GenSingle2D(
                static_cast<float>(wx) * f, static_cast<float>(wz) * f, seed);
            return base + static_cast<int>(v * static_cast<float>(amp));
        };
        int snowL = line(SNOW_JITTER_FREQ, SNOW_JITTER_SEED, 80, SNOW_JITTER);
        int stoneL = line(STONE_JITTER_FREQ, STONE_JITTER_SEED, 30, STONE_JITTER);
        int grassL = line(GRASS_JITTER_FREQ, GRASS_JITTER_SEED, -30, GRASS_JITTER);
        return blockForDepth(0, h - 1, snowL, stoneL, grassL);
    };

    // --- terrain pass ---
    for (int z = 0; z < Chunk::SIZE; ++z) {
        for (int x = 0; x < Chunk::SIZE; ++x) {
            int wx = worldCoord.x + x;
            int wz = worldCoord.z + z;
            int h = surfaceHeight(wx, wz, generator);
            auto line = [&](float f, int seed, int base, int amp) {
                float v = generator->GenSingle2D(
                    static_cast<float>(wx) * f, static_cast<float>(wz) * f, seed);
                return base + static_cast<int>(v * static_cast<float>(amp));
            };
            int snowLine = line(SNOW_JITTER_FREQ, SNOW_JITTER_SEED, 80, SNOW_JITTER);
            int stoneLine = line(STONE_JITTER_FREQ, STONE_JITTER_SEED, 30, STONE_JITTER);
            int grassLine = line(GRASS_JITTER_FREQ, GRASS_JITTER_SEED, -30, GRASS_JITTER);
            for (int y = 0; y < Chunk::SIZE; ++y) {
                int wy = worldCoord.y + y;
                if (wy >= h) {
                    continue;
                }
                int depth = (h - 1) - wy;
                chunk.set(x, y, z, blockForDepth(depth, wy, snowLine, stoneLine, grassLine));
            }
        }
    }
    // --- tree pass ---
    int cx0 = floorDiv(worldCoord.x - CANOPY_R, CELL);
    int cx1 = floorDiv(worldCoord.x + Chunk::SIZE + CANOPY_R, CELL);
    int cz0 = floorDiv(worldCoord.z - CANOPY_R, CELL);
    int cz1 = floorDiv(worldCoord.z + Chunk::SIZE + CANOPY_R, CELL);
    for (int cellX = cx0; cellX <= cx1; ++cellX) {
        for (int cellZ = cz0; cellZ <= cz1; ++cellZ) {
            std::uint32_t ha = hashCell(cellX, cellZ, 1);
            if ((ha & 7) != 0) {
                continue;
            }
            int bx = (cellX * CELL) + static_cast<int>((ha >> 3) & 7);
            int bz = (cellZ * CELL) + static_cast<int>((ha >> 6) & 7);
            if (surfaceBlockTop(bx, bz) != blocks::Grass) {
                continue;
            }
            int by = surfaceHeight(bx, bz, generator);
            stampTree(chunk, coord, bx, by, bz, ha);
        }
    }
}

}  // namespace hs::worldgen
