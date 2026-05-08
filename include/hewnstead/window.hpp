#pragma once

#include <string_view>

// Forward declaration: keep <GLFW/glfw3.h> out of public headers.
struct GLFWwindow;

namespace hs {

namespace {

// Window
constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 720;
constexpr int WINDOWED_X = 100;
constexpr int WINDOWED_Y = 100;
}  // namespace

class Input;  // forward declaration

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
    void toggleFullscreen();
    void requestClose();
    void attachInput(Input* input);

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
    Input* m_input = nullptr;
    int m_fbWidth = 0;
    int m_fbHeight = 0;
    int m_windowedX = WINDOWED_X;
    int m_windowedY = WINDOWED_Y;
    int m_windowedWidth = WINDOW_WIDTH;
    int m_windowedHeight = WINDOW_HEIGHT;
    bool m_fullscreen = false;

    // GLFW callback: forwards to the Window* stored as user pointer.
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void focusCallback(GLFWwindow* window, int focused);
};

}  // namespace hs
