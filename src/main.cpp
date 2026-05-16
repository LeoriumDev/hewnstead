#include "hewnstead/chunk_manager.hpp"
#include "hewnstead/mesher.hpp"
#include <hewnstead/camera.hpp>
#include <hewnstead/chunk_mesh.hpp>
#include <hewnstead/chunk_vertex.hpp>
#include <hewnstead/config.hpp>
#include <hewnstead/debug_overlay.hpp>
#include <hewnstead/imgui_runtime.hpp>
#include <hewnstead/input.hpp>
#include <hewnstead/shader.hpp>
#include <hewnstead/splash.hpp>
#include <hewnstead/window.hpp>

#include <glad/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <spdlog/spdlog.h>

#include <cstdlib>
#include <exception>
#include <imgui.h>

namespace {

void setupGlState() {
    // Enable depth test (nearest wins)
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Enable back-face culling (front-facing face is CCW from camera view)
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glClearColor(
        hs::config::CLEAR_R, hs::config::CLEAR_G, hs::config::CLEAR_B, hs::config::CLEAR_A);

    // Enable MSAA in rasterization
    glEnable(GL_MULTISAMPLE);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void handleSpecialKeys(hs::Input& input, hs::Window& window, bool& overlayVisible) {
    if (input.justPressed(GLFW_KEY_ESCAPE)) {
        window.requestClose();
    }

    if (input.justPressed(GLFW_KEY_F3)) {
        overlayVisible = !overlayVisible;
        window.setCursorMode(overlayVisible);

        if (overlayVisible) {
            int w = 0;
            int h = 0;
            glfwGetWindowSize(window.raw(), &w, &h);
            constexpr double CENTER = 0.5;
            glfwSetCursorPos(window.raw(), w * CENTER, h * CENTER);
        }

        input.clearKeys();
        input.resetMouseBaseline();
    }

    if (input.justPressed(GLFW_KEY_ENTER) &&
        (input.isDown(GLFW_KEY_LEFT_ALT) || input.isDown(GLFW_KEY_RIGHT_ALT))) {
        window.toggleFullscreen();
    }
}

void drawDebugUi(const hs::ChunkMesh& mesh,
                 GLuint64 samplesPassed,
                 GLint actualSamples,
                 bool& wireframe) {
    ImGui::Begin("Debug");
    ImGui::Text("Vertices: %d", mesh.vertexCount());
    ImGui::Text("Triangles: %d", mesh.vertexCount() / 3);
    ImGui::Text("Samples: %llu (~%llu px @ MSAA %dx)",
                samplesPassed,
                samplesPassed / actualSamples,
                actualSamples);
    ImGui::Checkbox("Wireframe", &wireframe);
    ImGui::End();
}

void renderScene(const hs::Shader& shader,
                 const hs::ChunkMesh& mesh,
                 const hs::Camera& camera,
                 float aspect,
                 bool wireframe) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);

    auto model = glm::mat4(1.0F);
    glm::mat4 view = camera.viewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(hs::config::FOV_DEGREES),
                                            aspect,
                                            hs::config::NEAR_PLANE,
                                            hs::config::FAR_PLANE);

    shader.use();
    shader.setMat4("u_model", model);
    shader.setMat4("u_view", view);
    shader.setMat4("u_projection", projection);

    mesh.draw();
}

}  // namespace

int main() {
    try {
        hs::printSplash();

        hs::Window window(hs::config::WINDOW_WIDTH, hs::config::WINDOW_HEIGHT, "Hewnstead");
        hs::Shader shader("assets/shaders/chunk.vert", "assets/shaders/chunk.frag");

        hs::ChunkManager chunkManager;
        hs::Chunk* chunk = chunkManager.loadChunk({.x = 0, .y = 0, .z = 0});
        assert(chunk != nullptr);

        for (int z = 0; z < hs::Chunk::SIZE; z++) {
            for (int y = 0; y < hs::Chunk::SIZE; y++) {
                for (int x = 0; x < hs::Chunk::SIZE; x++) {
                    chunk->set(x, y, z, hs::blocks::Dirt);
                }
            }
        }

        const auto vertices = hs::mesher::buildMesh(*chunk);
        spdlog::info("Mesher emitted {} vertices for fully-dirt chunk", vertices.size());

        hs::ChunkMesh chunkMesh;
        chunkMesh.upload(vertices);

        hs::Input input;
        hs::Camera camera;
        window.attachInput(&input);

        hs::ImguiRuntime runtime(window);
        input.connectImguiRuntime(&runtime);

        setupGlState();

        // Sample query
        GLuint sampleQuery = 0;
        glGenQueries(1, &sampleQuery);
        GLuint64 samplesPassed = 0;

        GLint actualSamples = 0;
        glGetIntegerv(GL_SAMPLES, &actualSamples);
        if (actualSamples == 0) {
            actualSamples = 1;
        }

        // Loop state
        double lastFrameTime = glfwGetTime();
        bool overlayVisible = false;
        bool wireframe = false;

        while (!window.shouldClose()) {
            // ─── Frame timing ───
            double now = glfwGetTime();
            auto dt = static_cast<float>(now - lastFrameTime);
            dt = std::min(dt, hs::config::DT_CAP);
            lastFrameTime = now;

            // ─── Input ───
            input.update(window.raw());
            window.pollEvents();
            handleSpecialKeys(input, window, overlayVisible);

            // ─── Simulation ───
            camera.update(input, dt);

            // ─── UI ───
            runtime.beginFrame();
            hs::drawCameraHud(camera, dt);
            if (overlayVisible) {
                drawDebugUi(chunkMesh, samplesPassed, actualSamples, wireframe);
            }

            // ─── Render ───
            glBeginQuery(GL_SAMPLES_PASSED, sampleQuery);
            renderScene(shader, chunkMesh, camera, window.aspect(), wireframe);
            glEndQuery(GL_SAMPLES_PASSED);
            glGetQueryObjectui64v(sampleQuery, GL_QUERY_RESULT, &samplesPassed);
            runtime.endFrame();

            // ─── Present ───
            window.swapBuffers();
        }

        glDeleteQueries(1, &sampleQuery);
    } catch (const std::exception& e) {
        spdlog::critical("Fatal: {}", e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
