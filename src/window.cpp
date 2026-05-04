#include <hewnstead/glcheck.hpp>
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

    glfwMakeContextCurrent(m_window);

    int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0) {
        glfwDestroyWindow(m_window);
        glfwTerminate();
        throw std::runtime_error("gladloadGL failed");
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

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void Window::pollEvents() {
    glfwPollEvents();
}
}  // namespace hs
