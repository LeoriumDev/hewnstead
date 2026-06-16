#include <hewnstead/render/mesher.hpp>
#include <hewnstead/world/block_id.hpp>
#include <hewnstead/world/chunk.hpp>

#include <array>
#include <cstddef>
#include <vector>

namespace hs::mesher {

namespace {

// Per-face 4-corner offsets (block-relative, [0, 1] range).
// Order: bottom-near → top-near → top-far → bottom-far (CCW when viewed from outside the face).
// Two triangles emitted per face share the hypotenuse: corners 0 and 2.
constexpr std::array<std::array<glm::vec3, 4>, FACE_COUNT> FACE_CORNERS = {{
    // East (+X) — looking from +X toward origin, CCW
    {{{1, 0, 0}, {1, 1, 0}, {1, 1, 1}, {1, 0, 1}}},
    // West (-X) — looking from -X toward origin, CCW
    {{{0, 0, 1}, {0, 1, 1}, {0, 1, 0}, {0, 0, 0}}},
    // Top (+Y) — looking from +Y down, CCW
    {{{0, 1, 0}, {0, 1, 1}, {1, 1, 1}, {1, 1, 0}}},
    // Bottom (-Y) — looking from -Y up, CCW
    {{{0, 0, 1}, {0, 0, 0}, {1, 0, 0}, {1, 0, 1}}},
    // South (+Z) — looking from +Z toward origin, CCW
    {{{0, 0, 1}, {1, 0, 1}, {1, 1, 1}, {0, 1, 1}}},
    // North (-Z) — looking from -Z toward origin, CCW
    {{{1, 0, 0}, {0, 0, 0}, {0, 1, 0}, {1, 1, 0}}},
}};

// Per-face neighbor cell offsets
constexpr std::array<glm::ivec3, FACE_COUNT> FACE_NEIGHBOR_OFFSETS = {{
    {+1, 0, 0},  // East   (+X)
    {-1, 0, 0},  // West   (-X)
    {0, +1, 0},  // Top    (+Y)
    {0, -1, 0},  // Bottom (-Y)
    {0, 0, +1},  // South  (+Z)
    {0, 0, -1},  // North  (-Z)
}};

// Per-face texture coords (UV per corner, matched to FACE_CORNERS).
// Side faces (East/West/South/North): up vector = +Y (world up).
// Top/Bottom faces: up vector = -Z (north).
constexpr std::array<std::array<glm::vec2, 4>, FACE_COUNT> FACE_UVS = {{
    {{{1.0F, 0.0F}, {1.0F, 1.0F}, {0.0F, 1.0F}, {0.0F, 0.0F}}},  // East   (+X)
    {{{1.0F, 0.0F}, {1.0F, 1.0F}, {0.0F, 1.0F}, {0.0F, 0.0F}}},  // West   (-X)
    {{{0.0F, 1.0F}, {0.0F, 0.0F}, {1.0F, 0.0F}, {1.0F, 1.0F}}},  // Top    (+Y)
    {{{1.0F, 0.0F}, {1.0F, 1.0F}, {0.0F, 1.0F}, {0.0F, 0.0F}}},  // Bottom (-Y)
    {{{0.0F, 0.0F}, {1.0F, 0.0F}, {1.0F, 1.0F}, {0.0F, 1.0F}}},  // South  (+Z)
    {{{0.0F, 0.0F}, {1.0F, 0.0F}, {1.0F, 1.0F}, {0.0F, 1.0F}}},  // North  (-Z)
}};

void emitFace(std::vector<ChunkVertex>& out, glm::vec3 base, Face face, float layer) {
    const auto idx = static_cast<std::size_t>(face);
    const auto& corners = FACE_CORNERS[idx];
    const auto& uv = FACE_UVS[idx];

    // Two triangles per quad, share hypotenuse (corner 0 and corner 2).
    // 1 ─── 2
    // │   / │
    // |  /  |
    // │ /   │
    // 0 ─── 3
    // Triangle 1: corner 0, 1, 2
    out.push_back({.position = base + corners[0], .uv = uv[0], .layer = layer});
    out.push_back({.position = base + corners[1], .uv = uv[1], .layer = layer});
    out.push_back({.position = base + corners[2], .uv = uv[2], .layer = layer});
    // Triangle 2: corner 0, 2, 3
    out.push_back({.position = base + corners[0], .uv = uv[0], .layer = layer});
    out.push_back({.position = base + corners[2], .uv = uv[2], .layer = layer});
    out.push_back({.position = base + corners[3], .uv = uv[3], .layer = layer});
}

}  // namespace

std::vector<ChunkVertex> buildMesh(const BlockAccessor& accessor) {
    std::vector<ChunkVertex> vertices;
    constexpr std::size_t VERTICES_PER_BLOCK = FACE_COUNT * 6;  // 2 triangles x 3 vertices for each
    vertices.reserve(Chunk::VOLUME * VERTICES_PER_BLOCK);

    for (int z = 0; z < Chunk::SIZE; ++z) {
        for (int y = 0; y < Chunk::SIZE; ++y) {
            for (int x = 0; x < Chunk::SIZE; ++x) {
                const BlockId block = accessor.get({x, y, z});
                if (block == blocks::Air) {
                    continue;
                }

                const glm::vec3 base{
                    static_cast<float>(x),
                    static_cast<float>(y),
                    static_cast<float>(z),
                };

                for (std::size_t f = 0; f < FACE_COUNT; f++) {
                    const glm::ivec3 offset = FACE_NEIGHBOR_OFFSETS[f];
                    const BlockId neighbor =
                        accessor.get({x + offset.x, y + offset.y, z + offset.z});
                    // Replace this inline check with isOpaque(neighbor) when transparent blocks is
                    // added (water, glass, leaves)
                    if (neighbor != blocks::Air) {
                        continue;  // hidden between two solid blocks; cull
                    }
                    emitFace(vertices,
                             base,
                             static_cast<Face>(f),
                             static_cast<float>(blocks::textureLayer(block, static_cast<Face>(f))));
                }
            }
        }
    }
    return vertices;
}

}  // namespace hs::mesher
