#pragma once

#include <hewnstead/world/chunk_manager.hpp>

namespace hs::collision {

// Returns true if the player AABB overlaps any solid block.
// feet: world-space bottom-center of the box (centered in x/z, base at y).
// size: box extents {width, height, depth}.
bool aabbHitsWorld(const ChunkManager& cm, glm::vec3 feet, glm::vec3 size);
bool aabbOverlapsCell(glm::vec3 feet, glm::vec3 size, glm::ivec3 cell);

}  // namespace hs::collision
