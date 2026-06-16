#pragma once

#include <hewnstead/world/chunk_manager.hpp>
#include <hewnstead/world/face.hpp>

#include <glm/vec3.hpp>

#include <optional>

namespace hs {

class Chunk;

struct RaycastHit {
    glm::ivec3 cell;           // voxel hit
    std::optional<Face> face;  // nullopt if origin was inside this cell
    float distance;            // 0 if face is nullopt
};

// To get real-world distance, make sure direction's length is 1 (normalized).
[[nodiscard]] std::optional<RaycastHit>
raycast(const ChunkManager&, glm::vec3 origin, glm::vec3 direction, float maxDistance);

}  // namespace hs
