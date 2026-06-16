#include <hewnstead/render/mesher.hpp>
#include <hewnstead/world/chunk.hpp>

#include <doctest/doctest.h>
#include <glm/glm.hpp>

TEST_CASE("Mesher: empty chunk emits zero vertices") {
    auto center = std::make_shared<hs::Chunk>();  // default-constructed, all air
    std::array<std::shared_ptr<hs::Chunk>, static_cast<std::size_t>(hs::Face::FACE_COUNT)>
        neighbors;
    hs::BlockAccessor accessor{center, neighbors};
    auto vertices = hs::mesher::buildMesh(accessor);
    CHECK(vertices.empty());
}

TEST_CASE("Mesher: isolated block emits 36 vertices (six air neighbors)") {
    auto center = std::make_shared<hs::Chunk>();
    std::array<std::shared_ptr<hs::Chunk>, static_cast<std::size_t>(hs::Face::FACE_COUNT)>
        neighbors;
    hs::BlockAccessor accessor{center, neighbors};
    center->set(5, 5, 5, hs::blocks::Dirt);
    auto vertices = hs::mesher::buildMesh(accessor);
    // 6 face × 6 vertex per face = 36
    CHECK(vertices.size() == 36);
}

TEST_CASE("Mesher: full chunk emits outer shell only (32^2 * 6 faces * 6 vertices)") {
    auto center = std::make_shared<hs::Chunk>();
    std::array<std::shared_ptr<hs::Chunk>, static_cast<std::size_t>(hs::Face::FACE_COUNT)>
        neighbors;
    hs::BlockAccessor accessor{center, neighbors};
    for (int z = 0; z < hs::Chunk::SIZE; ++z) {
        for (int y = 0; y < hs::Chunk::SIZE; ++y) {
            for (int x = 0; x < hs::Chunk::SIZE; ++x) {
                center->set(x, y, z, hs::blocks::Dirt);
            }
        }
    }
    auto vertices = hs::mesher::buildMesh(accessor);
    // 6 outer surfaces × 32² blocks per surface × 6 vertex per face
    constexpr std::size_t expected =
        std::size_t{6} * hs::Chunk::SIZE * hs::Chunk::SIZE * std::size_t{6};
    CHECK(vertices.size() == expected);  // 36,864
}

TEST_CASE("Mesher: single-block vertex positions stay within block bounds") {
    auto center = std::make_shared<hs::Chunk>();
    std::array<std::shared_ptr<hs::Chunk>, static_cast<std::size_t>(hs::Face::FACE_COUNT)>
        neighbors;
    hs::BlockAccessor accessor{center, neighbors};
    center->set(0, 0, 0, hs::blocks::Dirt);
    auto vertices = hs::mesher::buildMesh(accessor);

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
    auto center = std::make_shared<hs::Chunk>();
    std::array<std::shared_ptr<hs::Chunk>, static_cast<std::size_t>(hs::Face::FACE_COUNT)>
        neighbors;
    hs::BlockAccessor accessor{center, neighbors};
    center->set(10, 5, 20, hs::blocks::Dirt);
    auto vertices = hs::mesher::buildMesh(accessor);

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

TEST_CASE("Mesher: two adjacent blocks cull the shared face pair") {
    auto center = std::make_shared<hs::Chunk>();
    std::array<std::shared_ptr<hs::Chunk>, static_cast<std::size_t>(hs::Face::FACE_COUNT)>
        neighbors;
    hs::BlockAccessor accessor{center, neighbors};
    center->set(10, 10, 10, hs::blocks::Dirt);
    center->set(11, 10, 10, hs::blocks::Dirt);  // east neighbor of the first
    auto vertices = hs::mesher::buildMesh(accessor);

    // Two blocks × 6 faces × 6 vertex = 72 if unculled.
    // The shared face pair (first block's east + second block's west) is hidden.
    // 2 faces culled × 6 vertex = 12 culled. Expected: 72 - 12 = 60.
    CHECK(vertices.size() == 60);
}

TEST_CASE(
    "Mesher: block at chunk corner (0,0,0) emits all 6 faces (OOB neighbors treated as air)") {
    auto center = std::make_shared<hs::Chunk>();
    std::array<std::shared_ptr<hs::Chunk>, static_cast<std::size_t>(hs::Face::FACE_COUNT)>
        neighbors;
    hs::BlockAccessor accessor{center, neighbors};
    center->set(0, 0, 0, hs::blocks::Dirt);
    auto vertices = hs::mesher::buildMesh(accessor);

    // (0,0,0) has three OOB neighbors (-1 on each axis) and three in-bounds-air
    // neighbors. All six should be treated as air → all 6 faces emit.
    CHECK(vertices.size() == 36);
}
