#include <hewnstead/chunk.hpp>

#include <cassert>

namespace hs {

namespace {

std::size_t toIdx(int x, int y, int z) {
    return (static_cast<std::size_t>(z) * Chunk::SIZE * Chunk::SIZE) +
           (static_cast<std::size_t>(y) * Chunk::SIZE) + static_cast<std::size_t>(x);
}

bool inRange(int v) {
    return v >= 0 && v < Chunk::SIZE;
}

}  // namespace

BlockId Chunk::get(int x, int y, int z) const {
    assert(inRange(x));
    assert(inRange(y));
    assert(inRange(z));
    return m_blocks[toIdx(x, y, z)];
}

BlockId Chunk::getOrAir(int x, int y, int z) const {
    if (!inRange(x) || !inRange(y) || !inRange(z)) {
        return hs::blocks::Air;
    }
    return m_blocks[toIdx(x, y, z)];
}

void Chunk::set(int x, int y, int z, BlockId id) {
    assert(inRange(x));
    assert(inRange(y));
    assert(inRange(z));
    m_blocks[toIdx(x, y, z)] = id;
}

}  // namespace hs
