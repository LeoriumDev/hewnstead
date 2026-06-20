#include <hewnstead/physics/collision.hpp>

namespace hs::collision {

bool aabbHitsWorld(const ChunkManager& cm, glm::vec3 feet, glm::vec3 size) {
    glm::vec3 min = {feet.x - (size.x / 2), feet.y, feet.z - (size.z / 2)};
    glm::vec3 max = {feet.x + (size.x / 2), feet.y + size.y, feet.z + (size.z / 2)};
    auto int_floor = [](float n) -> int { return static_cast<int>(std::floor(n)); };
    for (int bx = int_floor(min.x); bx <= int_floor(max.x); bx++) {
        for (int by = int_floor(min.y); by <= int_floor(max.y); by++) {
            for (int bz = int_floor(min.z); bz <= int_floor(max.z); bz++) {
                if (cm.blockAt(bx, by, bz) != blocks::Air) {
                    return true;
                }
            }
        }
    }
    return false;
}

}  // namespace hs::collision
