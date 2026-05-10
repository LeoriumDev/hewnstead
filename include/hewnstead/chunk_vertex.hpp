#pragma once

#include <glm/vec3.hpp>

namespace hs {

struct ChunkVertex {
    glm::vec3 position;  // world-space, in voxel units (1 unit = 1 block)
    glm::vec3 color;     // RGB in [0, 1]
};

static_assert(sizeof(ChunkVertex) == 2 * sizeof(glm::vec3),
              "ChunkVertex must be exactly two glm::vec3 packed; "
              "layout breaks GL attribute pointers in ChunkMesh");
static_assert(std::is_standard_layout_v<ChunkVertex>,
              "ChunkVertex must be standard-layout for offsetof to be well-defined");

}  // namespace hs
