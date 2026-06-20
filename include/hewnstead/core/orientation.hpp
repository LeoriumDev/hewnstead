#pragma once

#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>

#include <cmath>

namespace hs::orientation {

inline constexpr glm::vec3 WORLD_UP{0.0F, 1.0F, 0.0F};

[[nodiscard]] inline glm::vec3 forward(float yaw, float pitch) {
    float cp = std::cos(pitch);
    return {cp * std::sin(yaw), std::sin(pitch), -cp * std::cos(yaw)};
}

[[nodiscard]] inline glm::vec3 horizontalForward(float yaw) {
    return forward(yaw, 0.0F);
}

[[nodiscard]] inline glm::vec3 right(float yaw) {
    return glm::normalize(glm::cross(forward(yaw, 0.0F), WORLD_UP));
}

}  // namespace hs::orientation
