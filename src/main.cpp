#include <hewnstead/core/config.hpp>
#include <hewnstead/core/input.hpp>
#include <hewnstead/core/splash.hpp>
#include <hewnstead/core/window.hpp>
#include <hewnstead/render/camera.hpp>
#include <hewnstead/render/chunk_mesh.hpp>
#include <hewnstead/render/debug_overlay.hpp>
#include <hewnstead/render/imgui_runtime.hpp>
#include <hewnstead/render/line_mesh.hpp>
#include <hewnstead/render/line_vertex.hpp>
#include <hewnstead/render/mesher.hpp>
#include <hewnstead/render/shader.hpp>
#include <hewnstead/render/texture_array.hpp>
#include <hewnstead/world/chunk_manager.hpp>
#include <hewnstead/world/raycast.hpp>

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

constexpr std::optional<hs::BlockId> keyToBlock(int key) {
    switch (key) {
    case GLFW_KEY_1:
        return hs::blocks::Stone;
    case GLFW_KEY_2:
        return hs::blocks::Dirt;
    case GLFW_KEY_3:
        return hs::blocks::Log;
    case GLFW_KEY_4:
        return hs::blocks::Planks;
    case GLFW_KEY_5:
        return hs::blocks::Grass;
    default:
        return std::nullopt;
    }
}

void handleBlockHotkeys(const hs::Input& input, hs::BlockId& selectedBlock) {
    for (int key = GLFW_KEY_1; key <= GLFW_KEY_5; ++key) {
        if (input.justPressed(key)) {
            if (auto blk = keyToBlock(key)) {
                selectedBlock = *blk;
            }
        }
    }
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
                 const hs::TextureArray& blockTextures,
                 float aspect,
                 bool wireframe,
                 const hs::LineMesh& lineMesh,
                 const hs::Shader& lineShader) {
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
    shader.setInt("u_blockTextures", 0);

    blockTextures.bind(0);
    mesh.draw();

    if (lineMesh.vertexCount() > 0) {
        lineShader.use();
        lineShader.setMat4("u_model", model);
        lineShader.setMat4("u_view", view);
        lineShader.setMat4("u_projection", projection);
        lineMesh.draw();
    }
}

void handleBreakBlock(const hs::Input& input,
                      hs::Chunk& chunk,
                      const std::optional<hs::RaycastHit>& lookingAt) {
    if (!input.mouseJustPressed(GLFW_MOUSE_BUTTON_LEFT) || input.imguiWantsMouse() || !lookingAt) {
        return;
    }
    chunk.set(lookingAt->cell.x, lookingAt->cell.y, lookingAt->cell.z, hs::blocks::Air);
}

void handlePlaceBlock(const hs::Input& input,
                      hs::Chunk& chunk,
                      const std::optional<hs::RaycastHit>& lookingAt,
                      hs::BlockId selectedBlock) {
    if (!input.mouseJustPressed(GLFW_MOUSE_BUTTON_RIGHT) || input.imguiWantsMouse() || !lookingAt ||
        !lookingAt->face) {  // inside-block hit, reject place
        return;
    }
    glm::ivec3 placeCell = lookingAt->cell + hs::faceNormal(*lookingAt->face);

    bool inBounds = placeCell.x >= 0 && placeCell.x < hs::Chunk::SIZE && placeCell.y >= 0 &&
                    placeCell.y < hs::Chunk::SIZE && placeCell.z >= 0 &&
                    placeCell.z < hs::Chunk::SIZE;
    if (!inBounds) {
        return;
    }

    if (chunk.getOrAir(placeCell.x, placeCell.y, placeCell.z) != hs::blocks::Air) {
        return;
    }
    chunk.set(placeCell.x, placeCell.y, placeCell.z, selectedBlock);
}

void remeshIfDirty(hs::Chunk& chunk, hs::ChunkMesh& chunkMesh) {
    if (!chunk.isDirty()) {
        return;
    }
    const auto vertices = hs::mesher::buildMesh(chunk);
    chunkMesh.upload(vertices);
    chunk.clearDirty();
}

std::array<hs::LineVertex, hs::config::CUBE_OUTLINE_VERTEX_COUNT>
cubeOutlineVertices(const glm::ivec3& cell, const glm::vec3& color) {
    const auto base = glm::vec3{cell};
    const std::array<glm::vec3, 8> c = {
        base + glm::vec3{0, 0, 0},
        base + glm::vec3{1, 0, 0},
        base + glm::vec3{1, 0, 1},
        base + glm::vec3{0, 0, 1},
        base + glm::vec3{0, 1, 0},
        base + glm::vec3{1, 1, 0},
        base + glm::vec3{1, 1, 1},
        base + glm::vec3{0, 1, 1},
    };
    const std::array<std::pair<int, int>, 12> edges = {{
        // bottom
        {0, 1},
        {1, 2},
        {2, 3},
        {3, 0},
        // top
        {4, 5},
        {5, 6},
        {6, 7},
        {7, 4},
        // vertical
        {0, 4},
        {1, 5},
        {2, 6},
        {3, 7},
    }};
    std::array<hs::LineVertex, hs::config::CUBE_OUTLINE_VERTEX_COUNT> out;
    for (std::size_t i = 0; i < 12; ++i) {
        out[2 * i] = {.position = c[edges[i].first], .color = color};
        out[(2 * i) + 1] = {.position = c[edges[i].second], .color = color};
    }
    return out;
}

void drawCrosshair(bool overlayVisible) {
    if (overlayVisible)
        return;

    ImDrawList* drawList = ImGui::GetForegroundDrawList();
    ImVec2 center = ImGui::GetIO().DisplaySize;
    center.x *= 0.5F;
    center.y *= 0.5F;

    constexpr float ARM = 10.0F;
    constexpr float THICKNESS = 2.0F;
    constexpr ImU32 COLOR = IM_COL32(255, 255, 255, 100);

    drawList->AddLine({center.x - ARM, center.y}, {center.x + ARM, center.y}, COLOR, THICKNESS);
    drawList->AddLine({center.x, center.y - ARM}, {center.x, center.y + ARM}, COLOR, THICKNESS);
}

constexpr glm::vec3 OUTLINE_COLOR{0.0F, 0.0F, 0.0F};

}  // namespace

int main() {
    try {
        hs::printSplash();

        hs::Window window(hs::config::WINDOW_WIDTH, hs::config::WINDOW_HEIGHT, "Hewnstead");
        hs::Shader shader("assets/shaders/chunk.vert", "assets/shaders/chunk.frag");
        hs::Shader lineShader("assets/shaders/line.vert", "assets/shaders/line.frag");

        constexpr std::array<std::string_view, 7> texturePaths = {
            "assets/textures/blocks/stone.png",       // layer 0
            "assets/textures/blocks/dirt.png",        // layer 1
            "assets/textures/blocks/log_side.png",    // layer 2
            "assets/textures/blocks/log_top.png",     // layer 3
            "assets/textures/blocks/planks.png",      // layer 4
            "assets/textures/blocks/grass_top.png",   // layer 5
            "assets/textures/blocks/grass_side.png",  // layer 6
        };
        hs::TextureArray blockTextures(texturePaths);

        hs::ChunkManager chunkManager;
        hs::Chunk* chunk = chunkManager.loadChunk({.x = 0, .y = 0, .z = 0});
        if (chunk == nullptr) {
            throw std::runtime_error("Failed to load initial chunk");
        }

        for (int z = 0; z < hs::Chunk::SIZE; z++) {
            for (int x = 0; x < hs::Chunk::SIZE; x++) {
                chunk->set(x, 0, z, hs::blocks::Log);
                chunk->set(x, 5, z, hs::blocks::Dirt);
                chunk->set(x, 10, z, hs::blocks::Planks);
                chunk->set(x, 15, z, hs::blocks::Grass);
                chunk->set(x, 20, z, hs::blocks::Stone);
            }
        }

        const auto vertices = hs::mesher::buildMesh(*chunk);

        hs::ChunkMesh chunkMesh;
        chunkMesh.upload(vertices);

        hs::LineMesh lineMesh;

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

        // Target block
        hs::BlockId selectedBlock = hs::blocks::Stone;
        const char* targetBlockName = nullptr;

        // Loop state
        bool overlayVisible = false;
        bool wireframe = false;

        double lastFrameTime = glfwGetTime();
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
            handleBlockHotkeys(input, selectedBlock);

            // ─── Simulation ───
            camera.update(input, dt);

            // ─── Targeting ───
            std::optional<hs::RaycastHit> lookingAt =
                hs::raycast(*chunk, camera.position(), camera.forward(), hs::config::MAX_REACH);
            static std::optional<glm::ivec3> lastOutlineCell;
            if (lookingAt && lookingAt->face) {
                if (!lastOutlineCell || *lastOutlineCell != lookingAt->cell) {
                    auto outline = cubeOutlineVertices(lookingAt->cell, OUTLINE_COLOR);
                    lineMesh.upload(std::span{outline});
                    lastOutlineCell = lookingAt->cell;
                }
            } else {
                if (lastOutlineCell) {
                    lineMesh.upload(std::span<const hs::LineVertex>{});
                    lastOutlineCell.reset();
                }
            }

            // ─── Block interaction ───
            handleBreakBlock(input, *chunk, lookingAt);
            handlePlaceBlock(input, *chunk, lookingAt, selectedBlock);
            remeshIfDirty(*chunk, chunkMesh);

            // ─── UI ───
            runtime.beginFrame();
            hs::drawCameraHud(camera, dt, lookingAt, targetBlockName, selectedBlock);
            if (overlayVisible) {
                drawDebugUi(chunkMesh, samplesPassed, actualSamples, wireframe);
            }

            // crosshair at screen center
            drawCrosshair(overlayVisible);

            // ─── Render ───
            glBeginQuery(GL_SAMPLES_PASSED, sampleQuery);
            renderScene(shader,
                        chunkMesh,
                        camera,
                        blockTextures,
                        window.aspect(),
                        wireframe,
                        lineMesh,
                        lineShader);
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
