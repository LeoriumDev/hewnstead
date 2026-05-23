#include <hewnstead/world/chunk.hpp>
#include <hewnstead/world/raycast.hpp>

#include <cmath>
#include <limits>

namespace hs {

namespace {

constexpr float FLOAT_INFINITY = std::numeric_limits<float>::infinity();

}  // namespace

std::optional<RaycastHit>  // NOLINTNEXTLINE(readability-function-cognitive-complexity)
raycast(const Chunk& chunk, glm::vec3 origin, glm::vec3 direction, float maxDistance) {
    // Integer voxel index containing origin
    int X = static_cast<int>(std::floor(origin.x));
    int Y = static_cast<int>(std::floor(origin.y));
    int Z = static_cast<int>(std::floor(origin.z));

    // Step direction per axis (+1 if ray moves in positive direction, -1 if negative)
    int stepX = (direction.x > 0) ? +1 : -1;
    int stepY = (direction.y > 0) ? +1 : -1;
    int stepZ = (direction.z > 0) ? +1 : -1;

    // Next boundary for each axis. Voxel index X occupies the real interval [X, X+1).
    // For positive direction, the next boundary is at X+1;
    // for negative direction, the next boundary is at X (the lower edge of the current voxel).
    float nextX = static_cast<float>(X) + (direction.x > 0 ? 1.0F : 0.0F);
    float nextY = static_cast<float>(Y) + (direction.y > 0 ? 1.0F : 0.0F);
    float nextZ = static_cast<float>(Z) + (direction.z > 0 ? 1.0F : 0.0F);

    // Parametric t at which the ray reaches the next boundary on each axis
    float tMaxX = (direction.x != 0) ? ((nextX - origin.x) / direction.x) : FLOAT_INFINITY;
    float tMaxY = (direction.y != 0) ? ((nextY - origin.y) / direction.y) : FLOAT_INFINITY;
    float tMaxZ = (direction.z != 0) ? ((nextZ - origin.z) / direction.z) : FLOAT_INFINITY;

    // t advance per one voxel width on each axis (infinity if direction is zero)
    float tDeltaX = (direction.x != 0) ? 1.0F / std::abs(direction.x) : FLOAT_INFINITY;
    float tDeltaY = (direction.y != 0) ? 1.0F / std::abs(direction.y) : FLOAT_INFINITY;
    float tDeltaZ = (direction.z != 0) ? 1.0F / std::abs(direction.z) : FLOAT_INFINITY;

    // Origin is inside a solid block. No entry face exists since the ray
    // didn't cross a boundary into this cell. face = nullopt signals this
    // to caller; place logic rejects, break logic still allows.
    if (chunk.getOrAir(X, Y, Z) != blocks::Air) {
        return RaycastHit{.cell = glm::ivec3{X, Y, Z}, .face = std::nullopt, .distance = 0.0F};
    }

    // Walk one voxel at a time.
    // Each iteration: find which axis's next boundary is closest (smallest tMax),
    // step the voxel index on that axis, update that axis's tMax for the boundary after the one
    // we just crossed, and record the face we entered through (opposite to step direction).
    float t = 0.0F;
    Face hitFace = Face::Top;  // arbitrary; always overwritten by first loop iteration before use
    while (true) {
        if (tMaxX < tMaxY && tMaxX < tMaxZ) {
            X += stepX;
            t = tMaxX;
            tMaxX += tDeltaX;
            hitFace = (stepX > 0) ? Face::West : Face::East;
        } else if (tMaxY < tMaxZ) {
            Y += stepY;
            t = tMaxY;
            tMaxY += tDeltaY;
            hitFace = (stepY > 0) ? Face::Bottom : Face::Top;
        } else {
            Z += stepZ;
            t = tMaxZ;
            tMaxZ += tDeltaZ;
            hitFace = (stepZ > 0) ? Face::North : Face::South;
        }

        // Termination: Out of reach
        if (t > maxDistance) {
            return std::nullopt;
        }

        // Termination: Hit
        if (chunk.getOrAir(X, Y, Z) != blocks::Air) {
            return RaycastHit{.cell = glm::ivec3{X, Y, Z}, .face = hitFace, .distance = t};
        }
    }
}

}  // namespace hs
