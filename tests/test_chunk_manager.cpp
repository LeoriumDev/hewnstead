#include <hewnstead/world/block_id.hpp>
#include <hewnstead/world/chunk_manager.hpp>

#include <doctest/doctest.h>

TEST_CASE("worldToChunk: positive coords") {
    auto c = hs::ChunkManager::worldToChunk(0, 0, 0);
    CHECK(c == hs::ChunkCoord{0, 0, 0});

    c = hs::ChunkManager::worldToChunk(31, 31, 31);
    CHECK(c == hs::ChunkCoord{0, 0, 0});

    c = hs::ChunkManager::worldToChunk(32, 0, 0);
    CHECK(c == hs::ChunkCoord{1, 0, 0});

    c = hs::ChunkManager::worldToChunk(63, 63, 63);
    CHECK(c == hs::ChunkCoord{1, 1, 1});
}

TEST_CASE("worldToChunk: negative coords") {
    auto c = hs::ChunkManager::worldToChunk(-1, -1, -1);
    CHECK(c == hs::ChunkCoord{-1, -1, -1});

    c = hs::ChunkManager::worldToChunk(-32, -32, -32);
    CHECK(c == hs::ChunkCoord{-1, -1, -1});

    c = hs::ChunkManager::worldToChunk(-33, 0, 0);
    CHECK(c == hs::ChunkCoord{-2, 0, 0});
}

TEST_CASE("worldToLocal: positive coords") {
    auto a = hs::ChunkManager::worldToLocal(0, 0, 0);
    CHECK(a == glm::ivec3{0, 0, 0});

    auto b = hs::ChunkManager::worldToLocal(31, 31, 31);
    CHECK(b == glm::ivec3{31, 31, 31});

    auto c = hs::ChunkManager::worldToLocal(32, 33, 34);
    CHECK(c == glm::ivec3{0, 1, 2});

    // Crosses chunk boundary in two of three axes.
    auto d = hs::ChunkManager::worldToLocal(63, 64, 65);
    CHECK(d == glm::ivec3{31, 0, 1});
}

TEST_CASE("worldToLocal: handles negative correctly") {
    auto a = hs::ChunkManager::worldToLocal(-1, 0, 0);
    CHECK(a == glm::ivec3{31, 0, 0});

    auto b = hs::ChunkManager::worldToLocal(-32, -1, 31);
    CHECK(b == glm::ivec3{0, 31, 31});
}

TEST_CASE("chunkToWorld") {
    CHECK(hs::ChunkManager::chunkToWorld({0, 0, 0}) == glm::ivec3{0, 0, 0});
    CHECK(hs::ChunkManager::chunkToWorld({1, 0, 0}) == glm::ivec3{32, 0, 0});
    CHECK(hs::ChunkManager::chunkToWorld({-1, 2, -3}) == glm::ivec3{-32, 64, -96});
}

TEST_CASE("chunkToWorld / worldToChunk round-trip") {
    for (auto c : {hs::ChunkCoord{.x = 0, .y = 0, .z = 0},
                   {.x = 1, .y = 2, .z = 3},
                   {.x = -5, .y = 7, .z = -13}}) {
        auto w = hs::ChunkManager::chunkToWorld(c);
        CHECK(hs::ChunkManager::worldToChunk(w.x, w.y, w.z) == c);
    }
}

TEST_CASE("worldPosToBlock: floor semantics on negative") {
    using hs::ChunkManager;

    // Positive fractional: floor and truncate agree.
    CHECK(ChunkManager::worldPosToBlock({0.5F, 0.5F, 0.5F}) == glm::ivec3{0, 0, 0});

    // Negative fractional: floor(-0.5) = -1, NOT static_cast<int>(-0.5) = 0.
    // This is the silent-bug case if anyone "optimizes" floor away.
    CHECK(ChunkManager::worldPosToBlock({-0.5F, 0.5F, 0.5F}) == glm::ivec3{-1, 0, 0});
    CHECK(ChunkManager::worldPosToBlock({-1.5F, -1.5F, -1.5F}) == glm::ivec3{-2, -2, -2});

    // Integer boundaries.
    CHECK(ChunkManager::worldPosToBlock({0.0F, 0.0F, 0.0F}) == glm::ivec3{0, 0, 0});
    CHECK(ChunkManager::worldPosToBlock({-1.0F, 0.0F, 0.0F}) == glm::ivec3{-1, 0, 0});
}

TEST_CASE("worldPosToChunk: composes worldPosToBlock + worldToChunk") {
    using hs::ChunkManager;

    CHECK(ChunkManager::worldPosToChunk({0.5F, 0.5F, 0.5F}) == hs::ChunkCoord{0, 0, 0});

    // Negative camera position must land in the negative chunk, not chunk 0.
    CHECK(ChunkManager::worldPosToChunk({-0.5F, 0.5F, 0.5F}) == hs::ChunkCoord{-1, 0, 0});

    // Positive boundary crossing.
    CHECK(ChunkManager::worldPosToChunk({32.5F, 0.0F, 0.0F}) == hs::ChunkCoord{1, 0, 0});

    // Negative boundary crossing: -32.5 floors to -33, which is chunk -2.
    CHECK(ChunkManager::worldPosToChunk({-32.5F, 0.0F, 0.0F}) == hs::ChunkCoord{-2, 0, 0});
}

TEST_CASE("ChunkManager load/get/unload") {
    hs::ChunkManager mgr;

    CHECK(mgr.chunkCount() == 0);
    CHECK(mgr.getChunk({0, 0, 0}) == nullptr);

    auto* a = mgr.loadChunk({.x = 0, .y = 0, .z = 0});
    REQUIRE(a != nullptr);
    CHECK(mgr.chunkCount() == 1);
    CHECK(mgr.getChunk({0, 0, 0}) == a);

    auto* b = mgr.loadChunk({.x = 1, .y = 2, .z = 3});
    REQUIRE(b != nullptr);
    CHECK(b != a);
    CHECK(mgr.chunkCount() == 2);

    mgr.unloadChunk({.x = 0, .y = 0, .z = 0});
    CHECK(mgr.chunkCount() == 1);
    CHECK(mgr.getChunk({0, 0, 0}) == nullptr);
    CHECK(mgr.getChunk({1, 2, 3}) == b);
}

TEST_CASE("loadChunk replaces existing chunk at same coord") {
    hs::ChunkManager mgr;

    auto* a = mgr.loadChunk({.x = 0, .y = 0, .z = 0});
    REQUIRE(a != nullptr);

    // Re-loading the same coord replaces (not emplace-if-missing).
    auto* b = mgr.loadChunk({.x = 0, .y = 0, .z = 0});
    REQUIRE(b != nullptr);

    CHECK(mgr.chunkCount() == 1);
    CHECK(b != a);                        // new allocation, distinct pointer
    CHECK(mgr.getChunk({0, 0, 0}) == b);  // get returns the replacement
    // `a` is now dangling — deliberately not dereferenced.
    // If anyone refactors loadChunk to emplace-if-missing semantics,
    // `b != a` will fail and this test will catch it.
}

TEST_CASE("Loaded chunk starts as all Air") {
    hs::ChunkManager mgr;
    auto* chunk = mgr.loadChunk({.x = 0, .y = 0, .z = 0});
    REQUIRE(chunk != nullptr);

    for (int z = 0; z < hs::Chunk::SIZE; ++z) {
        for (int y = 0; y < hs::Chunk::SIZE; ++y) {
            for (int x = 0; x < hs::Chunk::SIZE; ++x) {
                CHECK(chunk->get(x, y, z) == hs::blocks::Air);
            }
        }
    }
}
