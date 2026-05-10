#include <hewnstead/chunk_manager.hpp>

#include <bit>
#include <cmath>
#include <utility>

namespace hs {

namespace {

constexpr int CHUNK_SHIFT = std::countr_zero(static_cast<unsigned>(Chunk::SIZE));
constexpr int CHUNK_MASK = Chunk::SIZE - 1;

static_assert(std::has_single_bit(static_cast<unsigned>(Chunk::SIZE)),
              "Chunk::SIZE must be a power of two");

}  // namespace

Chunk* ChunkManager::loadChunk(ChunkCoord c) {
    auto chunk = std::make_unique<Chunk>();
    Chunk* raw = chunk.get();
    m_chunks[c] = std::move(chunk);
    return raw;
}

void ChunkManager::unloadChunk(ChunkCoord c) {
    m_chunks.erase(c);
}

Chunk* ChunkManager::getChunk(ChunkCoord c) {
    auto it = m_chunks.find(c);
    return it != m_chunks.end() ? it->second.get() : nullptr;
}

std::size_t ChunkManager::chunkCount() const {
    return m_chunks.size();
}

glm::ivec3 ChunkManager::chunkToWorld(ChunkCoord c) {
    return {c.x << CHUNK_SHIFT, c.y << CHUNK_SHIFT, c.z << CHUNK_SHIFT};
}

ChunkCoord ChunkManager::worldToChunk(int wx, int wy, int wz) {
    return {.x = wx >> CHUNK_SHIFT, .y = wy >> CHUNK_SHIFT, .z = wz >> CHUNK_SHIFT};
}

glm::ivec3 ChunkManager::worldToLocal(int wx, int wy, int wz) {
    return {wx & CHUNK_MASK, wy & CHUNK_MASK, wz & CHUNK_MASK};
}

glm::ivec3 ChunkManager::worldPosToBlock(const glm::vec3& worldPos) {
    return {
        static_cast<int>(std::floor(worldPos.x)),
        static_cast<int>(std::floor(worldPos.y)),
        static_cast<int>(std::floor(worldPos.z)),
    };
}

ChunkCoord ChunkManager::worldPosToChunk(const glm::vec3& worldPos) {
    glm::ivec3 b = worldPosToBlock(worldPos);
    return worldToChunk(b.x, b.y, b.z);
}

}  // namespace hs
