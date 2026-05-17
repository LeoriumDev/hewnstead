#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace hs {

struct ChunkVertex {
    glm::vec3 position;  // world-space, in voxel units (1 unit = 1 block)
    glm::vec2 uv;        // texture coord in [0, 1]
    float layer;         // texture array layer index
};

static_assert(sizeof(ChunkVertex) == sizeof(glm::vec3) + sizeof(glm::vec2) + sizeof(float),
              "ChunkVertex layout: position(vec3) + uv(vec2) + layer(float). "
              "Layout breaks GL attribute pointers in ChunkMesh");
static_assert(std::is_standard_layout_v<ChunkVertex>,
              "ChunkVertex must be standard-layout for offsetof to be well-defined");

}  // namespace hs
