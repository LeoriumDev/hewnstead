#include <hewnstead/world/block_id.hpp>
#include <hewnstead/world/chunk_manager.hpp>
#include <hewnstead/world/worldgen.hpp>

namespace hs::worldgen {

namespace {

constexpr float FREQ = 0.9F;
constexpr int BASE_HEIGHT = 32;
constexpr int AMPLITUDE = 32;
constexpr int SEED = 1337;

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
                if (wy < h) {
                    chunk.set(x, y, z, blocks::Stone);
                }
            }
        }
    }
}

}  // namespace hs::worldgen
