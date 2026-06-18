#pragma once
#include <hewnstead/world/chunk.hpp>
#include <hewnstead/world/chunk_coord.hpp>

#include <FastNoise/FastNoise.h>

namespace hs::worldgen {

void generateChunkTerrain(Chunk& chunk, ChunkCoord coord, const FastNoise::SmartNode<>& generator);

}  // namespace hs::worldgen
