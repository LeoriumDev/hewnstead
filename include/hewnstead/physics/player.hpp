#pragma once

#include "hewnstead/core/input.hpp"
#include "hewnstead/world/chunk_manager.hpp"

#include "glm/ext/vector_float3.hpp"

namespace hs {

struct Player {
    glm::vec3 position{};  // world space
    glm::vec3 velocity{};
    float yaw{};
    float pitch{};
    bool onGround{false};
    bool flying{false};
    float spaceTapTimer{0.0F};

    void update(const ChunkManager& cm, const Input& input, float dt);
    [[nodiscard]] glm::vec3 eyePosition() const;
};

constexpr glm::vec3 PLAYER_HITBOX = {0.6, 1.8, 0.6};

}  // namespace hs
