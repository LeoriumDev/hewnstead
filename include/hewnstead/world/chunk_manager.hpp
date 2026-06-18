#pragma once

#include <hewnstead/world/chunk.hpp>
#include <hewnstead/world/chunk_coord.hpp>

#include <glm/vec3.hpp>

#include <cstddef>
#include <memory>
#include <unordered_map>

namespace hs {

class ChunkManager {
public:
    ChunkManager() = default;
    ~ChunkManager() = default;

    ChunkManager(const ChunkManager&) = delete;
    ChunkManager& operator=(const ChunkManager&) = delete;
    ChunkManager(ChunkManager&&) = delete;
    ChunkManager& operator=(ChunkManager&&) = delete;

    // Lifecycle
    std::shared_ptr<Chunk> loadChunk(ChunkCoord c);
    void unloadChunk(ChunkCoord c);
    [[nodiscard]] std::shared_ptr<Chunk> getChunk(ChunkCoord c) const;
    [[nodiscard]] std::size_t chunkCount() const;

    // World-coord conversion
    [[nodiscard]] static glm::ivec3 chunkToWorld(ChunkCoord c);
    [[nodiscard]] static ChunkCoord worldToChunk(int wx, int wy, int wz);
    [[nodiscard]] static ChunkCoord worldToChunk(glm::ivec3 w);
    [[nodiscard]] static glm::ivec3 worldToLocal(int wx, int wy, int wz);
    [[nodiscard]] static glm::ivec3 worldToLocal(glm::ivec3 w);
    [[nodiscard]] static ChunkCoord worldPosToChunk(const glm::vec3& worldPos);
    [[nodiscard]] static glm::ivec3 worldPosToBlock(const glm::vec3& worldPos);

private:
    std::unordered_map<ChunkCoord, std::shared_ptr<Chunk>> m_chunks;
};

}  // namespace hs
