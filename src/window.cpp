#include <hewnstead/glcheck.hpp>
#include <hewnstead/input.hpp>
#include <hewnstead/window.hpp>

#include <glad/gl.h>

#include <GLFW/glfw3.h>

#include <spdlog/spdlog.h>

#include <stdexcept>

namespace hs {

namespace {

// file-scope function
void glfwErrorCallback(int code, const char* description) {
    spdlog::error("GLFW error {}: {}", code, (description != nullptr) ? description : "(null)");
}

}  // namespace

void Window::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (self == nullptr) {
        return;
    }
    self->m_fbWidth = width;
    self->m_fbHeight = height;
    glViewport(0, 0, width, height);
}

Window::Window(int width, int height, std::string_view title) {
    glfwSetErrorCallback(glfwErrorCallback);

    if (glfwInit() == 0) {
        throw std::runtime_error("glfwInit failed");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    std::string titleStr(title);
    m_window = glfwCreateWindow(width, height, titleStr.c_str(), nullptr, nullptr);
    if (m_window == nullptr) {
        glfwTerminate();
        throw std::runtime_error("glfwCreateWindow failed (see GLFW error above)");
    }

    setCursorMode(false);
    glfwMakeContextCurrent(m_window);

    int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0) {
        glfwDestroyWindow(m_window);
        glfwTerminate();
        throw std::runtime_error("gladLoadGL failed");
    }

    spdlog::info("GL_VERSION: {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    spdlog::info("GL_RENDERER: {}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));

    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallback);

    glfwGetFramebufferSize(m_window, &m_fbWidth, &m_fbHeight);
    glViewport(0, 0, m_fbWidth, m_fbHeight);

    // Enable VSync
    glfwSwapInterval(1);
}

Window::~Window() {
    if (m_window != nullptr) {
        glfwDestroyWindow(m_window);
    }
    glfwTerminate();
}

bool Window::shouldClose() const {
    return static_cast<bool>(glfwWindowShouldClose(m_window));
}

void Window::swapBuffers() {
    glfwSwapBuffers(m_window);
}

// pollEvents calls a free function but stays an instance method to keep
// Window's API surface coherent — callers always use `window.method()`.
// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void Window::pollEvents() {
    glfwPollEvents();
}

void Window::toggleFullscreen() {
    if (!m_fullscreen) {
        glfwGetWindowPos(m_window, &m_windowedX, &m_windowedY);
        glfwGetWindowSize(m_window, &m_windowedWidth, &m_windowedHeight);

        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor(m_window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        m_fullscreen = true;
    } else {
        glfwSetWindowMonitor(
            m_window, nullptr, m_windowedX, m_windowedY, m_windowedWidth, m_windowedHeight, 0);
        m_fullscreen = false;
    }
}

void Window::requestClose() {
    glfwSetWindowShouldClose(m_window, GLFW_TRUE);
}

void Window::attachInput(Input* input) {
    m_input = input;
    glfwSetKeyCallback(m_window, keyCallback);
}

void Window::keyCallback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/) {
    auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (self == nullptr || self->m_input == nullptr) {
        return;
    }
    self->m_input->onKeyEvent(key, action);
}

void Window::setCursorMode(bool visible) {
    glfwSetInputMode(m_window, GLFW_CURSOR, visible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
}

}  // namespace hs
