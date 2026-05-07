#pragma once

#include <GLFW/glfw3.h>

#include <array>

namespace hs {

class Input {
public:
    void update(GLFWwindow* window);

    void onKeyEvent(int key, int action);
    void clearKeys();
    [[nodiscard]] bool isDown(int key) const;
    [[nodiscard]] bool justPressed(int key) const;
    [[nodiscard]] bool justReleased(int key) const;

    [[nodiscard]] double mouseDx() const { return m_mouseDx; }
    [[nodiscard]] double mouseDy() const { return m_mouseDy; }

private:
    std::array<bool, GLFW_KEY_LAST + 1> m_keysNow{};
    std::array<bool, GLFW_KEY_LAST + 1> m_keysPrev{};
    double m_mouseXPrev = 0.0;
    double m_mouseYPrev = 0.0;
    double m_mouseDx = 0.0;
    double m_mouseDy = 0.0;

    bool m_firstMouse = true;
};

}  // namespace hs