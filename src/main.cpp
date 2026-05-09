#include <hewnstead/camera.hpp>
#include <hewnstead/imgui_runtime.hpp>
#include <hewnstead/input.hpp>
#include <hewnstead/mesh.hpp>
#include <hewnstead/shader.hpp>
#include <hewnstead/window.hpp>

#include <glad/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <spdlog/spdlog.h>

#include <cstdlib>
#include <exception>
#include <imgui.h>

namespace {

// Window
constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 720;

// Clear color (dark blue-grey)
constexpr float CLEAR_R = 0.10F;
constexpr float CLEAR_G = 0.10F;
constexpr float CLEAR_B = 0.12F;
constexpr float CLEAR_A = 1.00F;

// Frame timing
constexpr float DT_CAP = 0.1F;

// Camera projection
constexpr float FOV_DEGREES = 60.0F;
constexpr float NEAR_PLANE = 0.1F;
constexpr float FAR_PLANE = 1000.0F;

// Misc
constexpr double WINDOW_HALF = 0.5;

}  // namespace
int main() {
    try {
        spdlog::info("Hewnstead starting up...");

        hs::Window window(WINDOW_WIDTH, WINDOW_HEIGHT, "Hewnstead");
        hs::Shader shader("assets/shaders/triangle.vert", "assets/shaders/triangle.frag");
        hs::Mesh triangle = hs::Mesh::triangle();

        hs::Input input;
        hs::Camera camera;

        window.attachInput(&input);

        // Order is load-bearing: ImguiRuntime's constructor captures attachInput installed GLFW
        // callbacks
        hs::ImguiRuntime runtime(window);
        input.connectImguiRuntime(&runtime);

        glEnable(GL_DEPTH_TEST);
        glClearColor(CLEAR_R, CLEAR_G, CLEAR_B, CLEAR_A);

        double lastFrameTime = glfwGetTime();
        bool overlayVisible = false;

        while (!window.shouldClose()) {
            // Time
            double now = glfwGetTime();
            auto dt = static_cast<float>(now - lastFrameTime);
            dt = std::min(dt, DT_CAP);
            lastFrameTime = now;

            // Input
            input.update(window.raw());

            window.pollEvents();

            if (input.justPressed(GLFW_KEY_ESCAPE)) {
                window.requestClose();
            }

            // ~ toggle ImGui overlay for debugging
            if (input.justPressed(GLFW_KEY_GRAVE_ACCENT)) {
                overlayVisible = !overlayVisible;
                window.setCursorMode(overlayVisible);

                if (overlayVisible) {
                    int w;
                    int h;
                    glfwGetWindowSize(window.raw(), &w, &h);
                    glfwSetCursorPos(window.raw(), w * WINDOW_HALF, h * WINDOW_HALF);
                }

                input.clearKeys();
                input.resetMouseBaseline();
            }

            // F11 fullscreen
            bool fullscreenPressed = input.justPressed(GLFW_KEY_F11);
#ifdef __APPLE__
            if (input.justPressed(GLFW_KEY_F) && input.isDown(GLFW_KEY_LEFT_CONTROL) &&
                input.isDown(GLFW_KEY_LEFT_SUPER)) {
                fullscreenPressed = true;
            }
#endif
            if (fullscreenPressed) {
                window.toggleFullscreen();
            }

            camera.update(input, dt);

            // ImGui frame begin
            runtime.beginFrame();

            if (overlayVisible) {
                ImGui::ShowDemoWindow();
            }

            // Scene render
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            auto model = glm::mat4(1.0F);
            glm::mat4 view = camera.viewMatrix();
            glm::mat4 projection =
                glm::perspective(glm::radians(FOV_DEGREES), window.aspect(), NEAR_PLANE, FAR_PLANE);
            shader.use();
            shader.setMat4("u_model", model);
            shader.setMat4("u_view", view);
            shader.setMat4("u_projection", projection);
            triangle.draw();

            // ImGui frame end
            runtime.endFrame();

            window.swapBuffers();
        }
    } catch (const std::exception& e) {
        spdlog::critical("Fatal: {}", e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
