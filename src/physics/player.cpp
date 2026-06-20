#include "hewnstead/core/orientation.hpp"
#include <hewnstead/physics/player.hpp>

#include <glm/gtc/matrix_transform.hpp>

namespace hs {

namespace {

constexpr float SPEED = 20.0F;               // m/s
constexpr float GRAVITY = -28.0F;            // m/s^2
constexpr float DOUBLE_TAP_WINDOW = 0.28F;   // in seconds
constexpr float MOUSE_SENSITIVITY = 0.002F;  // rad/pixel
const float PITCH_LIMIT = glm::radians(89.0F);

constexpr float MOVEMENT_EPSILON = 0.0001F;

}  // namespace

void Player::update(const Input& input, float dt) {
    if (input.uiWantsKeyboard()) {
        return;
    }

    // Mouse look
    yaw += static_cast<float>(input.mouseDx()) * MOUSE_SENSITIVITY;
    pitch -= static_cast<float>(input.mouseDy()) * MOUSE_SENSITIVITY;
    pitch = std::clamp(pitch, -PITCH_LIMIT, PITCH_LIMIT);

    spaceTapTimer = std::max(0.0F, spaceTapTimer - dt);
    if (input.justPressed(GLFW_KEY_SPACE)) {
        if (spaceTapTimer > 0.0F) {
            flying = !flying;
            spaceTapTimer = 0.0F;
            if (flying) {
                velocity.y = 0.0F;
            }
        } else {
            spaceTapTimer = DOUBLE_TAP_WINDOW;
        }
    }

    // Movement
    glm::vec3 hf = orientation::horizontalForward(yaw);
    glm::vec3 r = orientation::right(yaw);

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
    if (flying) {
        if (input.isDown(GLFW_KEY_SPACE)) {
            wishDir += orientation::WORLD_UP;
        }
        if (input.isDown(GLFW_KEY_LEFT_SHIFT)) {
            wishDir -= orientation::WORLD_UP;
        }
        if (glm::length(wishDir) > MOVEMENT_EPSILON) {
            wishDir = glm::normalize(wishDir);
        }
        velocity = wishDir * SPEED;
    } else {
        if (glm::length(wishDir) > MOVEMENT_EPSILON) {
            wishDir = glm::normalize(wishDir);
        }
        velocity.x = wishDir.x * SPEED;
        velocity.z = wishDir.z * SPEED;
        velocity.y += GRAVITY * dt;
    }

    position += velocity * dt;
}

glm::vec3 Player::eyePosition() const {
    constexpr float EYE_HEIGHT = 1.5F;
    return {position.x, position.y + EYE_HEIGHT, position.z};
};

}  // namespace hs
