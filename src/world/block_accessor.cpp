#include <hewnstead/world/block_accessor.hpp>

namespace hs {

namespace {

bool inRange(int v) {
    return v >= 0 && v < Chunk::SIZE;
}

}  // namespace

BlockAccessor::BlockAccessor(const std::shared_ptr<Chunk>& center,
                             const std::array<std::shared_ptr<Chunk>, NEIGHBOR_COUNT>& neighbors)
    : m_center(center), m_neighbors(neighbors) {}

BlockId BlockAccessor::get(glm::ivec3 localCoord) const {
    if (inRange(localCoord.x) && inRange(localCoord.y) && inRange(localCoord.z)) {
        return m_center->get(localCoord.x, localCoord.y, localCoord.z);
    }

    // East Chunk
    if (localCoord.x >= Chunk::SIZE && inRange(localCoord.y) && inRange(localCoord.z)) {
        const auto& neighbor = m_neighbors[static_cast<std::size_t>(Face::East)];
        if (!neighbor) {
            return blocks::Air;
        }
        return neighbor->get(localCoord.x - Chunk::SIZE, localCoord.y, localCoord.z);
    }

    // West Chunk
    if (localCoord.x < 0 && inRange(localCoord.y) && inRange(localCoord.z)) {
        const auto& neighbor = m_neighbors[static_cast<std::size_t>(Face::West)];
        if (!neighbor) {
            return blocks::Air;
        }
        return neighbor->get(localCoord.x + Chunk::SIZE, localCoord.y, localCoord.z);
    }

    // Top Chunk
    if (localCoord.y >= Chunk::SIZE && inRange(localCoord.x) && inRange(localCoord.z)) {
        const auto& neighbor = m_neighbors[static_cast<std::size_t>(Face::Top)];
        if (!neighbor) {
            return blocks::Air;
        }
        return neighbor->get(localCoord.x, localCoord.y - Chunk::SIZE, localCoord.z);
    }

    // Bottom Chunk
    if (localCoord.y < 0 && inRange(localCoord.x) && inRange(localCoord.z)) {
        const auto& neighbor = m_neighbors[static_cast<std::size_t>(Face::Bottom)];
        if (!neighbor) {
            return blocks::Air;
        }
        return neighbor->get(localCoord.x, localCoord.y + Chunk::SIZE, localCoord.z);
    }

    // South Chunk
    if (localCoord.z >= Chunk::SIZE && inRange(localCoord.x) && inRange(localCoord.y)) {
        const auto& neighbor = m_neighbors[static_cast<std::size_t>(Face::South)];
        if (!neighbor) {
            return blocks::Air;
        }
        return neighbor->get(localCoord.x, localCoord.y, localCoord.z - Chunk::SIZE);
    }

    // North Chunk
    if (localCoord.z < 0 && inRange(localCoord.x) && inRange(localCoord.y)) {
        const auto& neighbor = m_neighbors[static_cast<std::size_t>(Face::North)];
        if (!neighbor) {
            return blocks::Air;
        }
        return neighbor->get(localCoord.x, localCoord.y, localCoord.z + Chunk::SIZE);
    }

    return blocks::Air;
}

}  // namespace hs
