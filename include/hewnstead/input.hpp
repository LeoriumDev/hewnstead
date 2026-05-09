#pragma once

#include <GLFW/glfw3.h>

#include <array>

namespace hs {

class ImguiRuntime;

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

    void connectImguiRuntime(const ImguiRuntime* runtime) { m_imgui = runtime; }
    [[nodiscard]] bool imguiWantsKeyboard() const;
    void resetMouseBaseline() { m_firstMouse = true; }

private:
    std::array<bool, GLFW_KEY_LAST + 1> m_keysNow{};
    std::array<bool, GLFW_KEY_LAST + 1> m_keysPrev{};
    double m_mouseXPrev = 0.0;
    double m_mouseYPrev = 0.0;
    double m_mouseDx = 0.0;
    double m_mouseDy = 0.0;

    bool m_firstMouse = true;

    const ImguiRuntime* m_imgui = nullptr;
};

}  // namespace hs
