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

    // Value-init the array to zero-fill all blocks to Air (= 0).
    Chunk() : m_blocks() {}

    [[nodiscard]] BlockId get(int x, int y, int z) const;

    // (x, y, z) must be in [0, SIZE).
    void set(int x, int y, int z, BlockId id);

    [[nodiscard]] std::span<const BlockId> blocks() const {
        return {m_blocks.data(), m_blocks.size()};
    }

private:
    std::array<BlockId, VOLUME> m_blocks;
};

}  // namespace hs