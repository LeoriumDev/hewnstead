#pragma once

#include <hewnstead/render/chunk_vertex.hpp>
#include <hewnstead/world/block_accessor.hpp>
#include <hewnstead/world/face.hpp>

#include <vector>

namespace hs {

class Chunk;

}  // namespace hs

namespace hs::mesher {

constexpr std::size_t FACE_COUNT = static_cast<std::size_t>(Face::FACE_COUNT);

[[nodiscard]] std::vector<ChunkVertex> buildMesh(const BlockAccessor& accessor);

}  // namespace hs::mesher
