#pragma once

#include <glm/ext/vector_int3.hpp>

#include <cstddef>
#include <functional>

namespace hs {

struct ChunkCoord {
    int x;
    int y;
    int z;

    friend bool operator==(const ChunkCoord&, const ChunkCoord&) = default;
};

[[nodiscard]] constexpr ChunkCoord operator+(ChunkCoord c, glm::ivec3 d) {
    return {.x = c.x + d.x, .y = c.y + d.y, .z = c.z + d.z};
}

}  // namespace hs

namespace std {

template <> struct hash<hs::ChunkCoord> {
    std::size_t operator()(const hs::ChunkCoord& c) const noexcept {
        // Boost's hash_combine pattern. The constant is the fractional part
        // of the golden ratio scaled to 32 bits — chosen for good bit
        // distribution when XOR-mixing.
        constexpr std::size_t HASH_COMBINE_GOLDEN =
            sizeof(std::size_t) == 8 ? 0x9e3779b97f4a7c15ULL : 0x9e3779b9UL;
        constexpr int HASH_COMBINE_LSHIFT = 6;
        constexpr int HASH_COMBINE_RSHIFT = 2;

        auto combine = [](std::size_t seed, int value) {
            return seed ^ (std::hash<int>{}(value) + HASH_COMBINE_GOLDEN +
                           (seed << HASH_COMBINE_LSHIFT) + (seed >> HASH_COMBINE_RSHIFT));
        };

        std::size_t h = std::hash<int>{}(c.x);
        h = combine(h, c.y);
        h = combine(h, c.z);
        return h;
    }
};

}  // namespace std
