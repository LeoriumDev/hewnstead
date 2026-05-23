#pragma once

#include <GLFW/glfw3.h>

#include <array>

namespace hs {

class Input {
public:
    void update(GLFWwindow* window);

    void onKeyEvent(int key, int action);
    void onMouseButtonEvent(int button, int action);

    void clearKeys();
    [[nodiscard]] bool isDown(int key) const;
    [[nodiscard]] bool justPressed(int key) const;
    [[nodiscard]] bool justReleased(int key) const;

    [[nodiscard]] bool isMouseDown(int button) const;
    [[nodiscard]] bool mouseJustPressed(int button) const;
    [[nodiscard]] bool mouseJustReleased(int button) const;

    [[nodiscard]] double mouseDx() const { return m_mouseDx; }
    [[nodiscard]] double mouseDy() const { return m_mouseDy; }

    [[nodiscard]] bool uiWantsKeyboard() const { return m_uiWantsKeyboard; };
    [[nodiscard]] bool uiWantsMouse() const { return m_uiWantsMouse; };
    void resetMouseBaseline() { m_firstMouse = true; }

    void setUiCapture(bool mouse, bool keyboard) {
        m_uiWantsMouse = mouse;
        m_uiWantsKeyboard = keyboard;
    }

private:
    std::array<bool, GLFW_KEY_LAST + 1> m_keysNow{};
    std::array<bool, GLFW_KEY_LAST + 1> m_keysPrev{};

    std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> m_mouseNow{};
    std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> m_mousePrev{};

    double m_mouseXPrev = 0.0;
    double m_mouseYPrev = 0.0;
    double m_mouseDx = 0.0;
    double m_mouseDy = 0.0;

    bool m_firstMouse = true;

    bool m_uiWantsMouse = false;
    bool m_uiWantsKeyboard = false;
};

}  // namespace hs
