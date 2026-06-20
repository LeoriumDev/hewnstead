#pragma once

#include "hewnstead/core/input.hpp"

#include "glm/ext/vector_float3.hpp"

namespace hs {

struct Player {
    glm::vec3 position{};  // world space
    glm::vec3 velocity{};
    float yaw{};
    float pitch{};
    bool onGround{false};

    void update(const Input& input, float dt);
    [[nodiscard]] glm::vec3 eyePosition() const;
};

}  // namespace hs
