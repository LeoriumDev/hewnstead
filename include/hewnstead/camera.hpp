#pragma once

#include <glm/glm.hpp>

namespace hs {

class Input;

class Camera {
public:
    Camera();

    void update(const Input& input, float dt);

    [[nodiscard]] glm::mat4 viewMatrix() const;

    [[nodiscard]] glm::vec3 position() const { return m_position; };
    [[nodiscard]] glm::vec3 forward() const;
    [[nodiscard]] glm::vec3 right() const;
    [[nodiscard]] glm::vec3 up() const;

private:
    [[nodiscard]] glm::vec3 horizontalForward() const;

    glm::vec3 m_position;
    float m_yaw;    // in radians
    float m_pitch;  // in radians
};

}  // namespace hs