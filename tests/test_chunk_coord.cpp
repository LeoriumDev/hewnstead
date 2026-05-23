#include <hewnstead/world/chunk_coord.hpp>

#include <doctest/doctest.h>

#include <unordered_map>

TEST_CASE("ChunkCoord equality") {
    hs::ChunkCoord a{.x = 1, .y = 2, .z = 3};
    hs::ChunkCoord b{.x = 1, .y = 2, .z = 3};
    hs::ChunkCoord c{.x = 1, .y = 2, .z = 4};

    CHECK(a == b);
    CHECK_FALSE(a == c);
    CHECK(a != c);
}

TEST_CASE("ChunkCoord hashes consistently") {
    std::hash<hs::ChunkCoord> h;
    hs::ChunkCoord a{.x = 1, .y = 2, .z = 3};
    hs::ChunkCoord b{.x = 1, .y = 2, .z = 3};

    CHECK(h(a) == h(b));
}

TEST_CASE("ChunkCoord hash distinguishes axis swaps") {
    std::hash<hs::ChunkCoord> h;
    hs::ChunkCoord a{.x = 1, .y = 2, .z = 3};
    hs::ChunkCoord b{.x = 2, .y = 1, .z = 3};

    CHECK(h(a) != h(b));
}

TEST_CASE("ChunkCoord works as unordered_map key") {
    std::unordered_map<hs::ChunkCoord, int> m;
    m[{.x = 1, .y = 2, .z = 3}] = 100;
    m[{.x = 4, .y = 5, .z = 6}] = 200;

    CHECK(m[{1, 2, 3}] == 100);
    CHECK(m[{4, 5, 6}] == 200);
    CHECK(m.size() == 2);
}
