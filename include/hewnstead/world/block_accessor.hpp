#pragma once

#include <hewnstead/world/block_id.hpp>
#include <hewnstead/world/chunk.hpp>
#include <hewnstead/world/face.hpp>

#include <glm/ext/vector_int3.hpp>

#include <array>
#include <memory>

namespace hs {

class BlockAccessor {
    static constexpr std::size_t NEIGHBOR_COUNT = static_cast<std::size_t>(Face::FACE_COUNT);

public:
    BlockAccessor(const std::shared_ptr<Chunk>& center,
                  const std::array<std::shared_ptr<Chunk>, NEIGHBOR_COUNT>& neighbors);
    [[nodiscard]] BlockId get(glm::ivec3 localCoord) const;

private:
    std::shared_ptr<Chunk> m_center;
    std::array<std::shared_ptr<Chunk>, NEIGHBOR_COUNT> m_neighbors;
};
}  // namespace hs
