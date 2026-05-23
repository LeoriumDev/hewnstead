#include <hewnstead/core/input.hpp>
#include <hewnstead/render/imgui_runtime.hpp>

namespace hs {

namespace {

inline std::size_t toIdx(int key) {
    return static_cast<std::size_t>(key);
}

}  // namespace

void Input::update(GLFWwindow* window) {
    // Snapshot previous frame's state, then updated by callbacks (onKeyEvent).
    m_keysPrev = m_keysNow;
    m_mousePrev = m_mouseNow;

    // Mouse delta with first-mouse fix.
    double curX;
    double curY;
    glfwGetCursorPos(window, &curX, &curY);

    if (m_imgui != nullptr && m_imgui->wantCaptureMouse()) {
        m_mouseDx = 0.0;
        m_mouseDy = 0.0;
        m_mouseXPrev = curX;
        m_mouseYPrev = curY;
        return;
    }

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

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
void Input::onMouseButtonEvent(int button, int action) {
    if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST) {
        return;
    }
    if (action == GLFW_PRESS) {
        m_mouseNow[toIdx(button)] = true;
    } else if (action == GLFW_RELEASE) {
        m_mouseNow[toIdx(button)] = false;
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

bool Input::isMouseDown(int button) const {
    if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST) {
        return false;
    }
    return m_mouseNow[toIdx(button)];
}

bool Input::mouseJustPressed(int button) const {
    if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST) {
        return false;
    }
    return m_mouseNow[toIdx(button)] && !m_mousePrev[toIdx(button)];
}

bool Input::mouseJustReleased(int button) const {
    if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST) {
        return false;
    }
    return !m_mouseNow[toIdx(button)] && m_mousePrev[toIdx(button)];
}

bool Input::imguiWantsKeyboard() const {
    return (m_imgui != nullptr) ? m_imgui->wantCaptureKeyboard() : false;
}

bool Input::imguiWantsMouse() const {
    return (m_imgui != nullptr) ? m_imgui->wantCaptureMouse() : false;
}

}  // namespace hs
