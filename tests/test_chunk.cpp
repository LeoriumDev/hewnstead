#include <hewnstead/world/chunk.hpp>

#include <doctest/doctest.h>

using hs::Chunk;
namespace blocks = hs::blocks;

TEST_CASE("Chunk default construction is all Air") {
    Chunk c;
    for (int z = 0; z < Chunk::SIZE; ++z) {
        for (int y = 0; y < Chunk::SIZE; ++y) {
            for (int x = 0; x < Chunk::SIZE; ++x) {
                CHECK(c.get(x, y, z) == blocks::Air);
            }
        }
    }
}

TEST_CASE("Chunk set / get round-trip") {
    Chunk c;

    SUBCASE("origin corner (0,0,0)") {
        c.set(0, 0, 0, blocks::Stone);
        CHECK(c.get(0, 0, 0) == blocks::Stone);
    }
    SUBCASE("far corner (31,31,31)") {
        c.set(31, 31, 31, blocks::Dirt);
        CHECK(c.get(31, 31, 31) == blocks::Dirt);
    }
    SUBCASE("interior (15, 7, 22)") {
        c.set(15, 7, 22, blocks::Stone);
        CHECK(c.get(15, 7, 22) == blocks::Stone);
    }
}

TEST_CASE("Chunk set does not affect neighbors") {
    Chunk c;
    c.set(10, 10, 10, blocks::Stone);

    // six face-neighbors stay Air
    CHECK(c.get(9, 10, 10) == blocks::Air);
    CHECK(c.get(11, 10, 10) == blocks::Air);
    CHECK(c.get(10, 9, 10) == blocks::Air);
    CHECK(c.get(10, 11, 10) == blocks::Air);
    CHECK(c.get(10, 10, 9) == blocks::Air);
    CHECK(c.get(10, 10, 11) == blocks::Air);

    // the cell itself is what we wrote
    CHECK(c.get(10, 10, 10) == blocks::Stone);
}

TEST_CASE("Chunk::blocks() span has size 32^3") {
    Chunk c;
    CHECK(c.blocks().size() == static_cast<std::size_t>(Chunk::SIZE * Chunk::SIZE * Chunk::SIZE));
}

TEST_CASE("Chunk storage is X-innermost (z*1024 + y*32 + x)") {
    Chunk c;
    c.set(1, 0, 0, blocks::Stone);  // x stride = 1
    c.set(0, 1, 0, blocks::Dirt);   // y stride = 32
    c.set(0, 0, 1, blocks::Stone);  // z stride = 32*32 = 1024

    auto data = c.blocks();
    CHECK(data[0] == blocks::Air);
    CHECK(data[1] == blocks::Stone);
    CHECK(data[32] == blocks::Dirt);
    CHECK(data[1024] == blocks::Stone);
}

TEST_CASE("Chunk: getOrAir returns Air for negative coordinates") {
    hs::Chunk chunk;
    chunk.set(0, 0, 0, hs::blocks::Dirt);
    CHECK(chunk.getOrAir(-1, 0, 0) == hs::blocks::Air);
    CHECK(chunk.getOrAir(0, -1, 0) == hs::blocks::Air);
    CHECK(chunk.getOrAir(0, 0, -1) == hs::blocks::Air);
}

TEST_CASE("Chunk: getOrAir returns Air for coordinates >= SIZE") {
    hs::Chunk chunk;
    chunk.set(31, 31, 31, hs::blocks::Dirt);
    CHECK(chunk.getOrAir(32, 0, 0) == hs::blocks::Air);
    CHECK(chunk.getOrAir(0, 32, 0) == hs::blocks::Air);
    CHECK(chunk.getOrAir(0, 0, 32) == hs::blocks::Air);
}

TEST_CASE("Chunk: getOrAir agrees with get for in-range coordinates") {
    hs::Chunk chunk;
    chunk.set(5, 10, 15, hs::blocks::Stone);
    CHECK(chunk.getOrAir(5, 10, 15) == hs::blocks::Stone);
    CHECK(chunk.getOrAir(4, 10, 15) == hs::blocks::Air);  // default
}
