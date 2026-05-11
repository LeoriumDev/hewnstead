#pragma once

#include <string_view>

// Forward declaration: keep <GLFW/glfw3.h> out of public headers.
struct GLFWwindow;

namespace hs {

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
    void setCursorMode(bool visible);
    void setWireframe(bool);  // GL_LINE, GL_FILL, GL_POINT
    void setVSync(bool);      // enum -1 adaptive 1 enable 0 disable
    void recenterCursor();

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
    int m_windowedX = 0;
    int m_windowedY = 0;
    int m_windowedWidth = 0;
    int m_windowedHeight = 0;
    bool m_fullscreen = false;

    // GLFW callback: forwards to the Window* stored as user pointer.
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
};

}  // namespace hs
