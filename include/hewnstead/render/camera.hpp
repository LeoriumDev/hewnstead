#pragma once

#include <glm/glm.hpp>

namespace hs {

class Input;

class Camera {
public:
    Camera() = default;

    void setView(glm::vec3 eyePos, float yaw, float pitch);

    [[nodiscard]] glm::mat4 viewMatrix() const;

    [[nodiscard]] float yaw() const { return m_yaw; }  // in radians

    [[nodiscard]] float pitch() const { return m_pitch; }  // in radians

    [[nodiscard]] glm::vec3 position() const { return m_position; }

    [[nodiscard]] glm::vec3 forward() const;
    [[nodiscard]] glm::vec3 right() const;
    [[nodiscard]] glm::vec3 up() const;

private:
    glm::vec3 m_position{};
    float m_yaw{0.0F};    // in radians
    float m_pitch{0.0F};  // in radians
};

}  // namespace hs
