#include <hewnstead/block_id.hpp>
#include <hewnstead/chunk.hpp>
#include <hewnstead/mesher.hpp>

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

// Per-face debug colors
constexpr std::array<glm::vec3, FACE_COUNT> FACE_COLORS = {{
    {0.20F, 0.40F, 0.85F},  // East = blue
    {0.30F, 0.70F, 0.30F},  // West = green
    {1.00F, 0.85F, 0.20F},  // Top = yellow
    {0.25F, 0.25F, 0.28F},  // Bottom = dark grey
    {0.95F, 0.55F, 0.15F},  // South = orange
    {0.85F, 0.20F, 0.20F},  // North = red
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

void emitFace(std::vector<ChunkVertex>& out, glm::vec3 base, Face face) {
    const auto idx = static_cast<std::size_t>(face);
    const auto& corners = FACE_CORNERS[idx];
    const glm::vec3 color = FACE_COLORS[idx];

    // Two triangles per quad, share hypotenuse (corner 0 and corner 2).
    // Triangle 1: corner 0, 1, 2
    out.push_back({.position = base + corners[0], .color = color});
    out.push_back({.position = base + corners[1], .color = color});
    out.push_back({.position = base + corners[2], .color = color});
    // Triangle 2: corner 0, 2, 3
    out.push_back({.position = base + corners[0], .color = color});
    out.push_back({.position = base + corners[2], .color = color});
    out.push_back({.position = base + corners[3], .color = color});
}

}  // namespace

std::vector<ChunkVertex> buildMesh(const Chunk& chunk) {
    std::vector<ChunkVertex> vertices;
    constexpr std::size_t VERTICES_PER_BLOCK =
        mesher::FACE_COUNT * 6;  // 2 triangles x 3 vertices for each
    vertices.reserve(Chunk::VOLUME * VERTICES_PER_BLOCK);

    for (int z = 0; z < Chunk::SIZE; z++) {
        for (int y = 0; y < Chunk::SIZE; ++y) {
            for (int x = 0; x < Chunk::SIZE; ++x) {
                if (chunk.get(x, y, z) == blocks::Air) {
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
                        chunk.getOrAir(x + offset.x, y + offset.y, z + offset.z);
                    // Replace this inline check with isOpaque(neighbor) when transparent blocks is
                    // added (water, glass, leaves)
                    if (neighbor != hs::blocks::Air) {
                        continue;  // hidden between two solid blocks; cull
                    }
                    emitFace(vertices, base, static_cast<Face>(f));
                }
            }
        }
    }
    return vertices;
}

}  // namespace hs::mesher
