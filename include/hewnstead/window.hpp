#pragma once

#include <string_view>

// Forward declaration: keep <GLFW/glfw3.h> out of public headers.
struct GLFWwindow;

namespace hs {

class Window {
public:
    Window(int width, int height, std::string_view title);
    ~Window();

    // Copying/moving would double-free the GL context.
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;

    // Frame loop helpers.
    [[nodiscard]] bool shouldClose() const;
    void swapBuffers();
    void pollEvents();

    // Framebuffer (pixels)
    [[nodiscard]] int framebufferWidth() const { return m_fbWidth; }
    [[nodiscard]] int framebufferHeight() const { return m_fbHeight; }
    [[nodiscard]] float aspect() const {
        return static_cast<float>(m_fbWidth) / static_cast<float>(m_fbHeight);
    }

    // Raw pointer for callbacks / GL function loading.
    [[nodiscard]] GLFWwindow* raw() const { return m_window; }

private:
    GLFWwindow* m_window = nullptr;
    int m_fbWidth = 0;
    int m_fbHeight = 0;

    // GLFW callback: forwards to the Window* stored as user pointer.
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
};

}  // namespace hs
