#include <hewnstead/camera.hpp>
#include <hewnstead/input.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <cmath>

namespace hs {

namespace {

constexpr float SPEED = 10.0F;               // m/s
constexpr float MOUSE_SENSITIVITY = 0.002F;  // rad/pixel
const float PITCH_LIMIT = glm::radians(89.0F);

constexpr float INITIAL_Z = 3.0F;
constexpr float MOVEMENT_EPSILON = 0.0001F;

constexpr glm::vec3 WORLD_UP{0.0F, 1.0F, 0.0F};

}  // namespace

Camera::Camera() : m_position(0.0F, 0.0F, INITIAL_Z), m_yaw(0.0F), m_pitch(0.0F) {}

glm::vec3 Camera::forward() const {
    float cp = std::cos(m_pitch);
    return {cp * std::sin(m_yaw), std::sin(m_pitch), -cp * std::cos(m_yaw)};
}

glm::vec3 Camera::right() const {
    return glm::normalize(glm::cross(forward(), WORLD_UP));
}

glm::vec3 Camera::up() const {
    return glm::cross(right(), forward());
}

glm::vec3 Camera::horizontalForward() const {
    glm::vec3 f = forward();
    f.y = 0.0F;
    return glm::normalize(f);
}

glm::mat4 Camera::viewMatrix() const {
    return glm::lookAt(m_position, m_position + forward(), WORLD_UP);
}

void Camera::update(const Input& input, float dt) {
    // Mouse look
    m_yaw += static_cast<float>(input.mouseDx()) * MOUSE_SENSITIVITY;
    m_pitch -= static_cast<float>(input.mouseDy()) * MOUSE_SENSITIVITY;
    m_pitch = std::clamp(m_pitch, -PITCH_LIMIT, PITCH_LIMIT);

    // Movement
    glm::vec3 hf = horizontalForward();
    glm::vec3 r = right();

    glm::vec3 wishDir(0.0F);
    if (input.isDown(GLFW_KEY_W)) {
        wishDir += hf;
    }
    if (input.isDown(GLFW_KEY_S)) {
        wishDir -= hf;
    }
    if (input.isDown(GLFW_KEY_D)) {
        wishDir += r;
    }
    if (input.isDown(GLFW_KEY_A)) {
        wishDir -= r;
    }

    if (glm::length(wishDir) > MOVEMENT_EPSILON) {
        wishDir = glm::normalize(wishDir);
        m_position += wishDir * SPEED * dt;
    }
    if (input.isDown(GLFW_KEY_SPACE)) {
        m_position.y += SPEED * dt;
    }
    if (input.isDown(GLFW_KEY_LEFT_SHIFT)) {
        m_position.y -= SPEED * dt;
    }
}

}  // namespace hs