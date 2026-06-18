#include <hewnstead/world/block_id.hpp>
#include <hewnstead/world/chunk_manager.hpp>
#include <hewnstead/world/worldgen.hpp>

namespace hs::worldgen {

namespace {

constexpr float FREQ = 0.5F;
constexpr int BASE_HEIGHT = 32;
constexpr int AMPLITUDE = 32;
constexpr int SEED = 1337;
constexpr int DIRT_DEPTH = 3;

constexpr BlockId blockForDepth(int depth) {
    if (depth == 0) {
        return blocks::Grass;
    }
    if (depth <= DIRT_DEPTH) {
        return blocks::Dirt;
    }
    return blocks::Stone;
}

}  // namespace

void generateChunkTerrain(Chunk& chunk, ChunkCoord coord, const FastNoise::SmartNode<>& generator) {
    for (int z = 0; z < Chunk::SIZE; z++) {
        for (int x = 0; x < Chunk::SIZE; x++) {
            glm::ivec3 worldCoord = ChunkManager::chunkToWorld(coord);
            int wx = worldCoord.x + x;
            int wz = worldCoord.z + z;
            float n = generator->GenSingle2D(
                static_cast<float>(wx) * FREQ, static_cast<float>(wz) * FREQ, SEED);
            int h = BASE_HEIGHT + static_cast<int>(n * AMPLITUDE);
            for (int y = 0; y < Chunk::SIZE; y++) {
                int wy = worldCoord.y + y;
                if (wy >= h) {
                    continue;
                }
                int depth = (h - 1) - wy;
                chunk.set(x, y, z, blockForDepth(depth));
            }
        }
    }
}

}  // namespace hs::worldgen
