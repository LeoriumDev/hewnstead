#include <hewnstead/core/input.hpp>
#include <hewnstead/core/orientation.hpp>
#include <hewnstead/render/camera.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include <cmath>

namespace hs {

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
void Camera::setView(glm::vec3 eyePos, float yaw, float pitch) {
    m_position = eyePos;
    m_yaw = yaw;
    m_pitch = pitch;
}

glm::vec3 Camera::forward() const {
    return orientation::forward(m_yaw, m_pitch);
}

glm::vec3 Camera::right() const {
    return glm::normalize(glm::cross(forward(), orientation::WORLD_UP));
}

glm::vec3 Camera::up() const {
    return glm::cross(right(), forward());
}

glm::mat4 Camera::viewMatrix() const {
    return glm::lookAt(m_position, m_position + forward(), orientation::WORLD_UP);
}

}  // namespace hs
