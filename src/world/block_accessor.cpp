#include <hewnstead/world/block_accessor.hpp>

namespace hs {

BlockAccessor::BlockAccessor(const std::shared_ptr<Chunk>& center,
                             const std::array<std::shared_ptr<Chunk>, NEIGHBOR_COUNT>& neighbors)
    : m_center(center), m_neighbors(neighbors) {}

BlockId BlockAccessor::get(glm::ivec3 localCoord) const {
    if (Chunk::inBounds(localCoord)) {
        return m_center->get(localCoord.x, localCoord.y, localCoord.z);
    }

    const std::array<std::tuple<bool, Face, glm::ivec3>, 6> borders = {{
        {localCoord.x >= Chunk::SIZE && Chunk::inBounds(localCoord.y) &&
             Chunk::inBounds(localCoord.z),
         Face::East,
         {-Chunk::SIZE, 0, 0}},
        {localCoord.x < 0 && Chunk::inBounds(localCoord.y) && Chunk::inBounds(localCoord.z),
         Face::West,
         {Chunk::SIZE, 0, 0}},
        {localCoord.y >= Chunk::SIZE && Chunk::inBounds(localCoord.x) &&
             Chunk::inBounds(localCoord.z),
         Face::Top,
         {0, -Chunk::SIZE, 0}},
        {localCoord.y < 0 && Chunk::inBounds(localCoord.x) && Chunk::inBounds(localCoord.z),
         Face::Bottom,
         {0, Chunk::SIZE, 0}},
        {localCoord.z >= Chunk::SIZE && Chunk::inBounds(localCoord.x) &&
             Chunk::inBounds(localCoord.y),
         Face::South,
         {0, 0, -Chunk::SIZE}},
        {localCoord.z < 0 && Chunk::inBounds(localCoord.x) && Chunk::inBounds(localCoord.y),
         Face::North,
         {0, 0, Chunk::SIZE}},
    }};

    for (const auto& [onBorder, face, offset] : borders) {
        if (!onBorder) {
            continue;
        }
        const auto& neighbor = m_neighbors[static_cast<std::size_t>(face)];
        if (!neighbor) {
            return blocks::Air;
        }
        return neighbor->get(localCoord + offset);
    }

    return blocks::Air;
}

}  // namespace hs
