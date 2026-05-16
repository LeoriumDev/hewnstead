#include <hewnstead/chunk.hpp>
#include <hewnstead/mesher.hpp>

#include <doctest/doctest.h>
#include <glm/glm.hpp>

TEST_CASE("Mesher: empty chunk emits zero vertices") {
    hs::Chunk chunk;  // default-constructed, all air
    auto vertices = hs::mesher::buildMesh(chunk);
    CHECK(vertices.empty());
}

TEST_CASE("Mesher: single block emits 36 vertices (no culling)") {
    hs::Chunk chunk;
    chunk.set(5, 5, 5, hs::blocks::Dirt);
    auto vertices = hs::mesher::buildMesh(chunk);
    // 6 face × 6 vertex per face = 36
    CHECK(vertices.size() == 36);
}

TEST_CASE("Mesher: full chunk emits 32^3 * 36 vertices") {
    hs::Chunk chunk;
    for (int z = 0; z < hs::Chunk::SIZE; ++z) {
        for (int y = 0; y < hs::Chunk::SIZE; ++y) {
            for (int x = 0; x < hs::Chunk::SIZE; ++x) {
                chunk.set(x, y, z, hs::blocks::Dirt);
            }
        }
    }
    auto vertices = hs::mesher::buildMesh(chunk);
    constexpr std::size_t expected =
        static_cast<std::size_t>(hs::Chunk::SIZE) * hs::Chunk::SIZE * hs::Chunk::SIZE * 36;
    CHECK(vertices.size() == expected);
}

TEST_CASE("Mesher: single-block vertex positions stay within block bounds") {
    hs::Chunk chunk;
    chunk.set(0, 0, 0, hs::blocks::Dirt);
    auto vertices = hs::mesher::buildMesh(chunk);

    for (const auto& v : vertices) {
        CHECK(v.position.x >= 0.0F);
        CHECK(v.position.x <= 1.0F);
        CHECK(v.position.y >= 0.0F);
        CHECK(v.position.y <= 1.0F);
        CHECK(v.position.z >= 0.0F);
        CHECK(v.position.z <= 1.0F);
    }
}

TEST_CASE("Mesher: non-origin block emits at correct world position") {
    hs::Chunk chunk;
    chunk.set(10, 5, 20, hs::blocks::Dirt);
    auto vertices = hs::mesher::buildMesh(chunk);

    // At least one vertex should be at the block's origin corner (10, 5, 20).
    bool foundOriginCorner = false;
    for (const auto& v : vertices) {
        if (v.position == glm::vec3(10.0F, 5.0F, 20.0F)) {
            foundOriginCorner = true;
            break;
        }
    }
    CHECK(foundOriginCorner);
}
