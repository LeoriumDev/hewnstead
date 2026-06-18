#include <hewnstead/world/chunk.hpp>

#include <cassert>

namespace hs {

namespace {

std::size_t toIdx(int x, int y, int z) {
    return (static_cast<std::size_t>(z) * Chunk::SIZE * Chunk::SIZE) +
           (static_cast<std::size_t>(y) * Chunk::SIZE) + static_cast<std::size_t>(x);
}

}  // namespace

BlockId Chunk::get(int x, int y, int z) const {
    assert(Chunk::inBounds(x));
    assert(Chunk::inBounds(y));
    assert(Chunk::inBounds(z));
    return m_blocks[toIdx(x, y, z)];
}

BlockId Chunk::get(glm::ivec3 c) const {
    assert(Chunk::inBounds(c.x));
    assert(Chunk::inBounds(c.y));
    assert(Chunk::inBounds(c.z));
    return m_blocks[toIdx(c.x, c.y, c.z)];
}

BlockId Chunk::getOrAir(int x, int y, int z) const {
    if (!Chunk::inBounds({x, y, z})) {
        return blocks::Air;
    }
    return m_blocks[toIdx(x, y, z)];
}

BlockId Chunk::getOrAir(glm::ivec3 c) const {
    if (!Chunk::inBounds(c)) {
        return blocks::Air;
    }
    return m_blocks[toIdx(c.x, c.y, c.z)];
}

void Chunk::set(int x, int y, int z, BlockId id) {
    assert(Chunk::inBounds({x, y, z}));
    m_blocks[toIdx(x, y, z)] = id;
    m_dirty = true;
}

void Chunk::set(glm::ivec3 c, BlockId id) {
    assert(Chunk::inBounds(c));
    m_blocks[toIdx(c.x, c.y, c.z)] = id;
    m_dirty = true;
}

}  // namespace hs
