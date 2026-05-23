#include <hewnstead/core/application.hpp>
#include <hewnstead/core/config.hpp>
#include <hewnstead/render/debug_overlay.hpp>
#include <hewnstead/render/line_vertex.hpp>
#include <hewnstead/render/mesher.hpp>
#include <hewnstead/world/chunk.hpp>

#include <glad/gl.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <array>
#include <imgui.h>
#include <span>
#include <stdexcept>
#include <string_view>

namespace hs {

namespace {

constexpr glm::vec3 OUTLINE_COLOR{0.0F, 0.0F, 0.0F};

constexpr std::array<std::string_view, 7> TEXTURE_PATHS = {
    "assets/textures/blocks/stone.png",       // layer 0
    "assets/textures/blocks/dirt.png",        // layer 1
    "assets/textures/blocks/log_side.png",    // layer 2
    "assets/textures/blocks/log_top.png",     // layer 3
    "assets/textures/blocks/planks.png",      // layer 4
    "assets/textures/blocks/grass_top.png",   // layer 5
    "assets/textures/blocks/grass_side.png",  // layer 6
};

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
                samplesPassed / static_cast<GLuint64>(actualSamples),
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
                      const std::optional<hs::RaycastHit>& lookingAt,
                      bool overlayVisible) {
    if (overlayVisible || !input.mouseJustPressed(GLFW_MOUSE_BUTTON_LEFT) || !lookingAt) {
        return;
    }
    chunk.set(lookingAt->cell.x, lookingAt->cell.y, lookingAt->cell.z, hs::blocks::Air);
}

void handlePlaceBlock(const hs::Input& input,
                      hs::Chunk& chunk,
                      const std::optional<hs::RaycastHit>& lookingAt,
                      hs::BlockId selectedBlock,
                      bool overlayVisible) {
    if (overlayVisible || !input.mouseJustPressed(GLFW_MOUSE_BUTTON_RIGHT) || !lookingAt ||
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
    for (std::size_t i = 0; i < hs::config::CUBE_EDGE_COUNT; ++i) {
        out[2 * i] = {.position = c[static_cast<size_t>(edges[i].first)], .color = color};
        out[(2 * i) + 1] = {.position = c[static_cast<size_t>(edges[i].second)], .color = color};
    }
    return out;
}

void drawCrosshair(bool overlayVisible) {
    if (overlayVisible) {
        return;
    }

    ImDrawList* drawList = ImGui::GetForegroundDrawList();
    ImVec2 center = ImGui::GetIO().DisplaySize;
    center.x /= 2;
    center.y /= 2;

    constexpr float ARM = 10.0F;
    constexpr float THICKNESS = 2.0F;
    constexpr ImU32 COLOR = IM_COL32(255, 255, 255, 100);

    drawList->AddLine({center.x - ARM, center.y}, {center.x + ARM, center.y}, COLOR, THICKNESS);
    drawList->AddLine({center.x, center.y - ARM}, {center.x, center.y + ARM}, COLOR, THICKNESS);
}

}  // namespace

Application::Application()
    : m_window(config::WINDOW_WIDTH, config::WINDOW_HEIGHT, config::TITLE),
      m_chunkShader(config::CHUNK_VERTEX_SHADER_PATH, config::CHUNK_FRAGMENT_SHADER_PATH),
      m_lineShader(config::LINE_VERTEX_SHADER_PATH, config::LINE_FRAGMENT_SHADER_PATH),
      m_blockTextures(TEXTURE_PATHS) {
    m_chunk = m_chunkManager.loadChunk({.x = 0, .y = 0, .z = 0});
    if (m_chunk == nullptr) {
        throw std::runtime_error("Failed to load initial chunk");
    }

    for (int z = 0; z < Chunk::SIZE; ++z) {
        for (int x = 0; x < Chunk::SIZE; ++x) {
            m_chunk->set(x, 0, z, blocks::Log);
            m_chunk->set(x, 5, z, blocks::Dirt);
            m_chunk->set(x, 10, z, blocks::Planks);
            m_chunk->set(x, 15, z, blocks::Grass);
            m_chunk->set(x, 20, z, blocks::Stone);
        }
    }

    m_chunkMesh.upload(mesher::buildMesh(*m_chunk));

    m_window.attachInput(&m_input);
    m_imgui.emplace(m_window);
    m_input.connectImguiRuntime(&*m_imgui);

    setupGlState();

    glGenQueries(1, &m_sampleQuery);
    glGetIntegerv(GL_SAMPLES, &m_actualSamples);
    if (m_actualSamples == 0) {
        m_actualSamples = 1;
    }

    m_lastFrameTime = glfwGetTime();
}

Application::~Application() {
    if (m_sampleQuery != 0) {
        glDeleteQueries(1, &m_sampleQuery);
    }
}

void Application::run() {
    while (!m_window.shouldClose()) {
        const auto now = glfwGetTime();
        auto dt = static_cast<float>(now - m_lastFrameTime);
        dt = std::min(dt, config::DT_CAP);
        m_lastFrameTime = now;

        update(dt);
        render(dt);
        m_window.swapBuffers();
    }
}

void Application::update(float dt) {
    // Input
    m_input.update(m_window.raw());
    m_window.pollEvents();
    handleSpecialKeys(m_input, m_window, m_overlayVisible);
    handleBlockHotkeys(m_input, m_selectedBlock);

    // Simulation
    m_camera.update(m_input, dt);

    // Targeting
    m_lookingAt = raycast(*m_chunk, m_camera.position(), m_camera.forward(), config::MAX_REACH);
    if (m_lookingAt && m_lookingAt->face) {
        if (!m_lastOutlineCell || *m_lastOutlineCell != m_lookingAt->cell) {
            auto outline = cubeOutlineVertices(m_lookingAt->cell, OUTLINE_COLOR);
            m_lineMesh.upload(std::span{outline});
            m_lastOutlineCell = m_lookingAt->cell;
        }
    } else if (m_lastOutlineCell) {
        m_lineMesh.upload(std::span<const LineVertex>{});
        m_lastOutlineCell.reset();
    }

    // Block interaction
    handleBreakBlock(m_input, *m_chunk, m_lookingAt, m_overlayVisible);
    handlePlaceBlock(m_input, *m_chunk, m_lookingAt, m_selectedBlock, m_overlayVisible);
    remeshIfDirty(*m_chunk, m_chunkMesh);
}

void Application::render(float dt) {
    // UI
    m_imgui->beginFrame();

    drawCameraHud(m_camera, dt, m_lookingAt, m_targetBlockName, m_selectedBlock);
    if (m_overlayVisible) {
        drawDebugUi(m_chunkMesh, m_samplesPassed, m_actualSamples, m_wireframe);
    }
    drawCrosshair(m_overlayVisible);

    // Scene
    glBeginQuery(GL_SAMPLES_PASSED, m_sampleQuery);
    renderScene(m_chunkShader,
                m_chunkMesh,
                m_camera,
                m_blockTextures,
                m_window.aspect(),
                m_wireframe,
                m_lineMesh,
                m_lineShader);
    glEndQuery(GL_SAMPLES_PASSED);
    glGetQueryObjectui64v(m_sampleQuery, GL_QUERY_RESULT, &m_samplesPassed);

    m_imgui->endFrame();
}

}  // namespace hs
