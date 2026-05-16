#pragma once

#include <hewnstead/chunk_vertex.hpp>

#include <vector>

namespace hs {

class Chunk;

}  // namespace hs

namespace hs::mesher {

enum class Face : std::uint8_t { East = 0, West, Top, Bottom, South, North, Count };

constexpr std::size_t FACE_COUNT = static_cast<std::size_t>(Face::Count);

[[nodiscard]] std::vector<ChunkVertex> buildMesh(const Chunk& chunk);

}  // namespace hs::mesher
