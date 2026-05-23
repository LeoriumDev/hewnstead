#include <hewnstead/world/chunk.hpp>
#include <hewnstead/world/raycast.hpp>

#include <doctest/doctest.h>
#include <glm/geometric.hpp>

using namespace hs;

TEST_CASE("raycast hits cell in front") {
    Chunk chunk;
    chunk.set(5, 0, 0, blocks::Stone);
    auto hit = raycast(chunk, {0.5F, 0.5F, 0.5F}, {1.0F, 0.0F, 0.0F}, 10.0F);
    REQUIRE(hit.has_value());
    CHECK(hit->cell == glm::ivec3{5, 0, 0});
    CHECK(hit->face == Face::West);
    CHECK(hit->distance == doctest::Approx(4.5F));
}

TEST_CASE("raycast misses if nothing in front") {
    Chunk chunk;
    auto hit = raycast(chunk, {0.5F, 0.5F, 0.5F}, {1.0F, 0.0F, 0.0F}, 10.0F);
    CHECK_FALSE(hit.has_value());
}

TEST_CASE("raycast respects max distance") {
    Chunk chunk;
    chunk.set(10, 0, 0, blocks::Stone);
    auto hit = raycast(chunk, {0.5F, 0.5F, 0.5F}, {1.0F, 0.0F, 0.0F}, 5.0F);
    CHECK_FALSE(hit.has_value());  // 10 - 0.5 = 9.5 > 5.0
}

TEST_CASE("raycast in negative direction") {
    Chunk chunk;
    chunk.set(1, 0, 0, blocks::Stone);
    auto hit = raycast(chunk, {3.5F, 0.5F, 0.5F}, {-1.0F, 0.0F, 0.0F}, 10.0F);
    REQUIRE(hit.has_value());
    CHECK(hit->cell == glm::ivec3{1, 0, 0});
    CHECK(hit->face == Face::East);  // ray came from +X side
}

TEST_CASE("raycast with zero direction component") {
    Chunk chunk;
    chunk.set(5, 5, 0, blocks::Stone);
    // Ray moves only in +X and +Y, no Z. Make sure the dir.z == 0 branch
    // doesn't divide by zero.
    auto hit = raycast(chunk, {0.5F, 0.5F, 0.5F}, {1.0F, 1.0F, 0.0F}, 20.0F);
    REQUIRE(hit.has_value());
    CHECK(hit->cell == glm::ivec3{5, 5, 0});
}

TEST_CASE("raycast diagonal direction") {
    Chunk chunk;
    chunk.set(3, 3, 3, blocks::Stone);
    glm::vec3 dir = glm::normalize(glm::vec3{1.0F, 1.0F, 1.0F});
    auto hit = raycast(chunk, {0.5F, 0.5F, 0.5F}, dir, 10.0F);
    REQUIRE(hit.has_value());
    CHECK(hit->cell == glm::ivec3{3, 3, 3});
}

TEST_CASE("raycast origin inside solid block") {
    Chunk chunk;
    chunk.set(5, 5, 5, blocks::Stone);
    // Origin at center of the stone block.
    auto hit = raycast(chunk, {5.5F, 5.5F, 5.5F}, {1.0F, 0.0F, 0.0F}, 10.0F);
    REQUIRE(hit.has_value());
    CHECK(hit->cell == glm::ivec3{5, 5, 5});
    CHECK_FALSE(hit->face.has_value());  // no entry face for inside hit
    CHECK(hit->distance == 0.0F);
}
