#include <hewnstead/input.hpp>

namespace hs {

namespace {

inline std::size_t toIdx(int key) {
    return static_cast<std::size_t>(key);
}

}  // namespace

void Input::update(GLFWwindow* window) {
    // Snapshot previous frame's state, then updated by callbacks (onKeyEvent).
    m_keysPrev = m_keysNow;

    // Mouse delta with first-mouse fix.
    double curX;
    double curY;
    glfwGetCursorPos(window, &curX, &curY);

    if (m_firstMouse) {
        m_mouseDx = 0.0;
        m_mouseDy = 0.0;
        m_firstMouse = false;
    } else {
        m_mouseDx = curX - m_mouseXPrev;
        m_mouseDy = curY - m_mouseYPrev;
    }

    m_mouseXPrev = curX;
    m_mouseYPrev = curY;
}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
void Input::onKeyEvent(int key, int action) {
    if (key < 0 || key > GLFW_KEY_LAST) {
        return;
    }
    if (action == GLFW_PRESS) {
        m_keysNow[toIdx(key)] = true;
    } else if (action == GLFW_RELEASE) {
        m_keysNow[toIdx(key)] = false;
    }
}

void Input::clearKeys() {
    m_keysNow.fill(false);
}

bool Input::isDown(int key) const {
    if (key < 0 || key > GLFW_KEY_LAST) {
        return false;
    }
    return m_keysNow[toIdx(key)];
}

bool Input::justPressed(int key) const {
    if (key < 0 || key > GLFW_KEY_LAST) {
        return false;
    }
    return m_keysNow[toIdx(key)] && !m_keysPrev[toIdx(key)];
}

bool Input::justReleased(int key) const {
    if (key < 0 || key > GLFW_KEY_LAST) {
        return false;
    }
    return !m_keysNow[toIdx(key)] && m_keysPrev[toIdx(key)];
}

}  // namespace hs