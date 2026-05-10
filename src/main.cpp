#include <hewnstead/camera.hpp>
#include <hewnstead/chunk_mesh.hpp>
#include <hewnstead/chunk_vertex.hpp>
#include <hewnstead/debug_overlay.hpp>
#include <hewnstead/imgui_runtime.hpp>
#include <hewnstead/input.hpp>
#include <hewnstead/shader.hpp>
#include <hewnstead/window.hpp>

#include <glad/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <spdlog/spdlog.h>

#include <array>
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

// Cube colors
constexpr glm::vec3 COLOR_TOP{1.0F, 0.85F, 0.2F};
constexpr glm::vec3 COLOR_BOTTOM{0.25F, 0.25F, 0.28F};
constexpr glm::vec3 COLOR_NORTH{0.85F, 0.2F, 0.2F};
constexpr glm::vec3 COLOR_SOUTH{0.95F, 0.55F, 0.15F};
constexpr glm::vec3 COLOR_EAST{0.2F, 0.4F, 0.85F};
constexpr glm::vec3 COLOR_WEST{0.3F, 0.7F, 0.3F};

// Cube vertices
constexpr std::array<hs::ChunkVertex, 36> CUBE_VERTICES = {{
    // East face (+X, blue) — looking from +X toward origin, CCW
    {.position = {1.0F, 0.0F, 0.0F}, .color = COLOR_EAST},
    {.position = {1.0F, 1.0F, 0.0F}, .color = COLOR_EAST},
    {.position = {1.0F, 1.0F, 1.0F}, .color = COLOR_EAST},
    {.position = {1.0F, 0.0F, 0.0F}, .color = COLOR_EAST},
    {.position = {1.0F, 1.0F, 1.0F}, .color = COLOR_EAST},
    {.position = {1.0F, 0.0F, 1.0F}, .color = COLOR_EAST},

    // West face (-X, green) — looking from -X toward origin, CCW
    {.position = {0.0F, 0.0F, 1.0F}, .color = COLOR_WEST},
    {.position = {0.0F, 1.0F, 1.0F}, .color = COLOR_WEST},
    {.position = {0.0F, 1.0F, 0.0F}, .color = COLOR_WEST},
    {.position = {0.0F, 0.0F, 1.0F}, .color = COLOR_WEST},
    {.position = {0.0F, 1.0F, 0.0F}, .color = COLOR_WEST},
    {.position = {0.0F, 0.0F, 0.0F}, .color = COLOR_WEST},

    // Top face (+Y, yellow) — looking from +Y down, CCW
    {.position = {0.0F, 1.0F, 0.0F}, .color = COLOR_TOP},
    {.position = {0.0F, 1.0F, 1.0F}, .color = COLOR_TOP},
    {.position = {1.0F, 1.0F, 1.0F}, .color = COLOR_TOP},
    {.position = {0.0F, 1.0F, 0.0F}, .color = COLOR_TOP},
    {.position = {1.0F, 1.0F, 1.0F}, .color = COLOR_TOP},
    {.position = {1.0F, 1.0F, 0.0F}, .color = COLOR_TOP},

    // Bottom face (-Y, dark grey) — looking from -Y up, CCW
    {.position = {0.0F, 0.0F, 1.0F}, .color = COLOR_BOTTOM},
    {.position = {0.0F, 0.0F, 0.0F}, .color = COLOR_BOTTOM},
    {.position = {1.0F, 0.0F, 0.0F}, .color = COLOR_BOTTOM},
    {.position = {0.0F, 0.0F, 1.0F}, .color = COLOR_BOTTOM},
    {.position = {1.0F, 0.0F, 0.0F}, .color = COLOR_BOTTOM},
    {.position = {1.0F, 0.0F, 1.0F}, .color = COLOR_BOTTOM},

    // South face (+Z, orange) — looking from +Z toward origin, CCW
    {.position = {0.0F, 0.0F, 1.0F}, .color = COLOR_SOUTH},
    {.position = {1.0F, 0.0F, 1.0F}, .color = COLOR_SOUTH},
    {.position = {1.0F, 1.0F, 1.0F}, .color = COLOR_SOUTH},
    {.position = {0.0F, 0.0F, 1.0F}, .color = COLOR_SOUTH},
    {.position = {1.0F, 1.0F, 1.0F}, .color = COLOR_SOUTH},
    {.position = {0.0F, 1.0F, 1.0F}, .color = COLOR_SOUTH},

    // North face (-Z, red) — looking from -Z toward origin, CCW
    {.position = {1.0F, 0.0F, 0.0F}, .color = COLOR_NORTH},
    {.position = {0.0F, 0.0F, 0.0F}, .color = COLOR_NORTH},
    {.position = {0.0F, 1.0F, 0.0F}, .color = COLOR_NORTH},
    {.position = {1.0F, 0.0F, 0.0F}, .color = COLOR_NORTH},
    {.position = {0.0F, 1.0F, 0.0F}, .color = COLOR_NORTH},
    {.position = {1.0F, 1.0F, 0.0F}, .color = COLOR_NORTH},
}};

}  // namespace

int main() {
    try {
        spdlog::info("Hewnstead starting up...");

        hs::Window window(WINDOW_WIDTH, WINDOW_HEIGHT, "Hewnstead");
        hs::Shader shader("assets/shaders/chunk.vert", "assets/shaders/chunk.frag");
        hs::ChunkMesh cube((std::span<const hs::ChunkVertex>(CUBE_VERTICES)));

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

            // Alt+Enter fullscreen toggle
            if (input.justPressed(GLFW_KEY_ENTER) &&
                (input.isDown(GLFW_KEY_LEFT_ALT) || input.isDown(GLFW_KEY_RIGHT_ALT))) {
                window.toggleFullscreen();
            }

            camera.update(input, dt);

            // ImGui frame begin
            runtime.beginFrame();

            hs::drawCameraHud(camera, dt);

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
            cube.draw();

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
