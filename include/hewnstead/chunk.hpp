#pragma once

#include <hewnstead/block_id.hpp>

#include <array>
#include <cstddef>
#include <span>

namespace hs {

static_assert(hs::blocks::Air == 0, "Chunk's default-constructed state relies on Air being zero");

class Chunk {
public:
    static constexpr int SIZE = 32;
    static constexpr std::size_t VOLUME = static_cast<std::size_t>(SIZE) * SIZE * SIZE;

    Chunk() = default;

    // (x, y, z) must be in [0, SIZE).
    [[nodiscard]] BlockId get(int x, int y, int z) const;

    // (x, y, z) may be anywhere; OOB is Air
    [[nodiscard]] BlockId getOrAir(int x, int y, int z) const;

    // (x, y, z) must be in [0, SIZE). Auto-marks chunk dirty.
    void set(int x, int y, int z, BlockId id);

    [[nodiscard]] std::span<const BlockId> blocks() const {
        return {m_blocks.data(), m_blocks.size()};
    }

    [[nodiscard]] bool isDirty() const { return m_dirty; }
    void makeDirty() { m_dirty = true; }
    void clearDirty() { m_dirty = false; }

private:
    // Value-initialized to zero on default construction; zero == Air,
    // so every new chunk is all-air with no per-cell loop.
    std::array<BlockId, VOLUME> m_blocks{};

    // start dirty so first frame builds mesh
    bool m_dirty = true;
};

}  // namespace hs
