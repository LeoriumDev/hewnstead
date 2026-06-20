#include <hewnstead/core/orientation.hpp>
#include <hewnstead/physics/collision.hpp>
#include <hewnstead/physics/player.hpp>

#include <glm/gtc/matrix_transform.hpp>

namespace hs {

namespace {

constexpr float SPEED = 5.0F;                // m/s
constexpr float FLY_SPEED = 15.0F;           // m/s
constexpr float GRAVITY = -28.0F;            // m/s^2
constexpr float DOUBLE_TAP_WINDOW = 0.28F;   // in seconds
constexpr float MOUSE_SENSITIVITY = 0.002F;  // rad/pixel
const float PITCH_LIMIT = glm::radians(89.0F);

constexpr float MOVEMENT_EPSILON = 0.0001F;
constexpr float SKIN_WIDTH = 0.00001F;
constexpr float JUMP_HEIGHT = 1.25F;

constexpr float ACCEL = 50.0F;
constexpr float AIR_ACCEL = 10.0F;
constexpr float FRICTION = 30.0F;

glm::vec3 moveToward(glm::vec3 current, glm::vec3 target, float maxStep) {
    glm::vec3 diff = target - current;
    if (glm::length(diff) <= maxStep) {
        return target;
    }
    return current + glm::normalize(diff) * maxStep;
}

}  // namespace

void Player::update(const ChunkManager& cm, const Input& input, float dt) {
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
        const bool hasInput = input.isDown(GLFW_KEY_SPACE) || input.isDown(GLFW_KEY_LEFT_SHIFT) ||
                              input.isDown(GLFW_KEY_W) || input.isDown(GLFW_KEY_S) ||
                              input.isDown(GLFW_KEY_A) || input.isDown(GLFW_KEY_D);
        float step = (hasInput ? ACCEL : FRICTION) * dt;
        glm::vec3 target = hasInput ? wishDir * FLY_SPEED : glm::vec3{0.0F, 0.0F, 0.0F};
        velocity = moveToward(velocity, target, step);
    } else {
        if (glm::length(wishDir) > MOVEMENT_EPSILON) {
            wishDir = glm::normalize(wishDir);
        }

        const bool hasInput = input.isDown(GLFW_KEY_W) || input.isDown(GLFW_KEY_S) ||
                              input.isDown(GLFW_KEY_A) || input.isDown(GLFW_KEY_D);
        glm::vec3 horiz{velocity.x, 0.0F, velocity.z};
        glm::vec3 target = hasInput ? glm::vec3{wishDir.x * SPEED, 0.0F, wishDir.z * SPEED}
                                    : glm::vec3{0.0F, 0.0F, 0.0F};
        float step;
        if (onGround) {
            step = (hasInput ? ACCEL : FRICTION) * dt;
        } else {
            step = (hasInput ? AIR_ACCEL : FRICTION) * dt;
        }
        horiz = moveToward(horiz, target, step);
        velocity.x = horiz.x;
        velocity.z = horiz.z;

        if (onGround && input.isDown(GLFW_KEY_SPACE)) {
            velocity.y = std::sqrt(2 * std::abs(GRAVITY) * JUMP_HEIGHT);
        }
        velocity.y += GRAVITY * dt;
        onGround = false;
    }

    for (auto axis : {0, 1, 2}) {
        position[axis] += velocity[axis] * dt;
        constexpr glm::vec3 lowExtent = {PLAYER_HITBOX.x / 2, 0, PLAYER_HITBOX.z / 2};
        constexpr glm::vec3 highExtent = {
            PLAYER_HITBOX.x / 2, PLAYER_HITBOX.y, PLAYER_HITBOX.z / 2};

        if (collision::aabbHitsWorld(cm, position, PLAYER_HITBOX)) {
            if (velocity[axis] >= 0.0F) {
                position[axis] =
                    std::floor(position[axis] + highExtent[axis]) - highExtent[axis] - SKIN_WIDTH;
            } else if (velocity[axis] < 0.0F) {
                position[axis] =
                    std::floor(position[axis] - lowExtent[axis] + 1) + lowExtent[axis] + SKIN_WIDTH;
            }
            if (axis == 1 && velocity[axis] < 0.0F) {
                onGround = true;
            }
            velocity[axis] = 0.0F;
        }
    }
}

glm::vec3 Player::eyePosition() const {
    constexpr float EYE_HEIGHT = 1.7F;
    return {position.x, position.y + EYE_HEIGHT, position.z};
};

}  // namespace hs
