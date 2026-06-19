#include <hewnstead/core/application.hpp>
#include <hewnstead/core/config.hpp>
#include <hewnstead/core/profiler.hpp>
#include <hewnstead/render/debug_overlay.hpp>
#include <hewnstead/render/frustum.hpp>
#include <hewnstead/render/line_vertex.hpp>
#include <hewnstead/render/mesher.hpp>
#include <hewnstead/world/chunk.hpp>
#include <hewnstead/world/worldgen.hpp>

#include <glad/gl.h>

#include <GLFW/glfw3.h>

#include <FastNoise/Generators/Simplex.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <spdlog/fmt/bundled/format.h>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <span>
#include <stdexcept>
#include <string_view>

namespace hs {

namespace {

constexpr glm::vec3 OUTLINE_COLOR{0.0F, 0.0F, 0.0F};

constexpr std::array<std::string_view, 20> TEXTURE_PATHS = {
    "assets/textures/blocks/stone.png",              // layer 0
    "assets/textures/blocks/dirt.png",               // layer 1
    "assets/textures/blocks/log_side.png",           // layer 2
    "assets/textures/blocks/log_top.png",            // layer 3
    "assets/textures/blocks/planks.png",             // layer 4
    "assets/textures/blocks/grass_top.png",          // layer 5
    "assets/textures/blocks/grass_side.png",         // layer 6
    "assets/textures/blocks/bricks.png",             // layer 7
    "assets/textures/blocks/clay.png",               // layer 8
    "assets/textures/blocks/coal_ore.png",           // layer 9
    "assets/textures/blocks/cobblestone.png",        // layer 10
    "assets/textures/blocks/copper_ore.png",         // layer 11
    "assets/textures/blocks/gravel.png",             // layer 12
    "assets/textures/blocks/iron_ore.png",           // layer 13
    "assets/textures/blocks/leaves.png",             // layer 14
    "assets/textures/blocks/mossy_cobblestone.png",  // layer 15
    "assets/textures/blocks/sand.png",               // layer 16
    "assets/textures/blocks/snow.png",               // layer 17
    "assets/textures/blocks/stone_bricks.png",       // layer 18
    "assets/textures/blocks/thatch.png",             // layer 19
};

constexpr float FPS_SAMPLE_INTERVAL = 0.5F;

void setupGlState() {
    // Enable depth test (nearest wins)
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Enable back-face culling (front-facing face is CCW from camera view)
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glClearColor(config::CLEAR_R, config::CLEAR_G, config::CLEAR_B, config::CLEAR_A);

    // Enable MSAA in rasterization
    glEnable(GL_MULTISAMPLE);

    // Explicit default
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Enable sRGB for texture
    glEnable(GL_FRAMEBUFFER_SRGB);
}

void handleBlockHotkeys(const Input& input, BlockId& selectedBlock) {
    for (int key = GLFW_KEY_0; key <= GLFW_KEY_9; ++key) {
        if (input.justPressed(key)) {
            selectedBlock = static_cast<BlockId>(key - GLFW_KEY_0 +
                                                 (input.isDown(GLFW_KEY_LEFT_CONTROL) ? 9 : 0));
        }
    }
}

void handleSpecialKeys(hs::Input& input,
                       bool& overlayVisible,
                       hs::Window& window,
                       bool& wireframe) {
    if (input.justPressed(GLFW_KEY_ESCAPE)) {
        window.requestClose();
    }

    if (input.justPressed(GLFW_KEY_F4)) {
        wireframe = !wireframe;
    }

    if (input.justPressed(GLFW_KEY_F3)) {
        overlayVisible = !overlayVisible;
        input.clearKeys();
        input.resetMouseBaseline();
    }

    if (input.justPressed(GLFW_KEY_ENTER) &&
        (input.isDown(GLFW_KEY_LEFT_ALT) || input.isDown(GLFW_KEY_RIGHT_ALT))) {
        window.toggleFullscreen();
    }
}

int renderScene(const Shader& shader,
                const std::unordered_map<ChunkCoord, ChunkMesh>& meshes,
                const Camera& camera,
                const TextureArray& blockTextures,
                float aspect,
                bool wireframe,
                const LineMesh& lineMesh,
                const Shader& lineShader) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);

    glm::mat4 view = camera.viewMatrix();
    glm::mat4 projection = glm::perspective(
        glm::radians(config::FOV_DEGREES), aspect, config::NEAR_PLANE, config::FAR_PLANE);

    Frustum frustum{projection * view};

    shader.use();
    shader.setMat4("u_view", view);
    shader.setMat4("u_projection", projection);
    shader.setInt("u_blockTextures", 0);
    blockTextures.bind(0);

    int drawn = 0;
    for (const auto& [coord, mesh] : meshes) {
        glm::vec3 min = glm::vec3(ChunkManager::chunkToWorld(coord));
        glm::vec3 max = min + glm::vec3(Chunk::SIZE);
        if (!frustum.isVisible(min, max)) {
            continue;
        }
        const glm::vec3 origin = min;
        const glm::mat4 model = glm::translate(glm::mat4(1.0F), origin);
        shader.setMat4("u_model", model);
        mesh.draw();
        drawn++;
    }

    if (lineMesh.vertexCount() > 0) {
        lineShader.use();
        lineShader.setMat4("u_model", glm::mat4(1.0F));
        lineShader.setMat4("u_view", view);
        lineShader.setMat4("u_projection", projection);
        lineMesh.draw();
    }

    return drawn;
}

// Border edit: the neighbor chunk's facing surface may need re-meshing.
void markEditDirty(ChunkManager& cm, const glm::ivec3 cell) {
    constexpr int MAX = Chunk::SIZE - 1;
    const ChunkCoord coord = ChunkManager::worldToChunk(cell);
    const glm::ivec3 local = ChunkManager::worldToLocal(cell);

    // clang-format off
    const std::array<std::pair<bool, glm::ivec3>, 6> borders = {{
        {local.x == 0, {-1, 0, 0}}, {local.x == MAX, {1, 0, 0}},
        {local.y == 0, {0, -1, 0}}, {local.y == MAX, {0, 1, 0}},
        {local.z == 0, {0, 0, -1}}, {local.z == MAX, {0, 0, 1}},
    }};
    // clang-format on

    for (const auto& [onBorder, offset] : borders) {
        if (!onBorder) {
            continue;
        }
        if (auto n = cm.getChunk(coord + offset)) {
            n->makeDirty();
        }
    }
}

// Edits one world cell. Marks the edited chunk + any bordering neighbor dirty.
// onlyIfAir: skip if the target isn't air (place semantics). Returns whether it changed.
bool setWorldBlock(ChunkManager& cm, glm::ivec3 cell, BlockId block, bool onlyIfAir = false) {
    const ChunkCoord coord = ChunkManager::worldToChunk(cell);
    const auto chunk = cm.getChunk(coord);
    if (!chunk) {
        return false;
    }
    const glm::ivec3 local = ChunkManager::worldToLocal(cell);
    if (onlyIfAir && chunk->get(local) != blocks::Air) {
        return false;
    }
    chunk->set(local, block);
    markEditDirty(cm, cell);
    return true;
}

void handleBreakBlock(const Input& input,
                      ChunkManager& cm,
                      const std::optional<RaycastHit>& lookingAt) {
    if (!input.mouseJustPressed(GLFW_MOUSE_BUTTON_LEFT) || !lookingAt) {
        return;
    }
    setWorldBlock(cm, lookingAt->cell, blocks::Air);
}

void handlePlaceBlock(const Input& input,
                      ChunkManager& cm,
                      const std::optional<RaycastHit>& lookingAt,
                      BlockId selectedBlock) {
    if (!input.mouseJustPressed(GLFW_MOUSE_BUTTON_RIGHT) || !lookingAt || !lookingAt->face) {
        return;
    }
    const glm::ivec3 placeCell = lookingAt->cell + hs::faceNormal(*lookingAt->face);
    setWorldBlock(cm, placeCell, selectedBlock, true);
}

void handlePickBlock(const Input& input,
                     ChunkManager& cm,
                     const std::optional<RaycastHit>& lookingAt,
                     BlockId& selectedBlock) {
    if (!input.mouseJustPressed(GLFW_MOUSE_BUTTON_MIDDLE) || !lookingAt) {
        return;
    }
    const std::shared_ptr<Chunk> chunk = cm.getChunk(ChunkManager::worldToChunk(lookingAt->cell));
    if (!chunk) {
        return;
    }
    BlockId target = chunk->getOrAir(lookingAt->cell);
    if (target != blocks::Air) {
        selectedBlock = target;
    }
}

std::array<LineVertex, config::CUBE_OUTLINE_VERTEX_COUNT>
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
    // clang-format off
    const std::array<std::pair<int, int>, 12> edges = {{
        {0, 1}, {1, 2}, {2, 3}, {3, 0},  // bottom
        {4, 5}, {5, 6}, {6, 7}, {7, 4},  // top
        {0, 4}, {1, 5}, {2, 6}, {3, 7},  // vertical
    }};
    // clang-format on
    std::array<LineVertex, config::CUBE_OUTLINE_VERTEX_COUNT> out;
    for (std::size_t i = 0; i < config::CUBE_EDGE_COUNT; ++i) {
        out[2 * i] = {.position = c[static_cast<size_t>(edges[i].first)], .color = color};
        out[(2 * i) + 1] = {.position = c[static_cast<size_t>(edges[i].second)], .color = color};
    }
    return out;
}

void drawCrosshair() {
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

std::vector<ChunkCoord> initialGridCoord() {
    std::vector<ChunkCoord> coords;
    for (int cz = -10; cz <= 10; ++cz) {
        for (int cy = -5; cy <= 5; ++cy) {
            for (int cx = -10; cx <= 10; ++cx) {
                coords.emplace_back(cx, cy, cz);
            }
        }
    }
    return coords;
}

}  // namespace

Application::Application()
    : m_appStart(std::chrono::steady_clock::now()),
      m_window(config::WINDOW_WIDTH, config::WINDOW_HEIGHT, config::TITLE),
      m_chunkShader(config::CHUNK_VERTEX_SHADER_PATH, config::CHUNK_FRAGMENT_SHADER_PATH),
      m_lineShader(config::LINE_VERTEX_SHADER_PATH, config::LINE_FRAGMENT_SHADER_PATH),
      m_blockTextures(TEXTURE_PATHS) {
    auto bodyStart = std::chrono::steady_clock::now();
    spdlog::info("[profile] context+shaders+textures: {:.1f} ms",
                 std::chrono::duration<double, std::milli>(bodyStart - m_appStart).count());
    auto simplex = FastNoise::New<FastNoise::Simplex>();
    auto fbm = FastNoise::New<FastNoise::FractalFBm>();
    fbm->SetSource(simplex);
    fbm->SetOctaveCount(4);
    fbm->SetLacunarity(2.0F);
    fbm->SetGain(0.4F);

    {
        ScopedTimer t{"worldgen"};
        m_coords = initialGridCoord();
        for (const ChunkCoord& coord : m_coords) {
            const auto chunk = m_chunkManager.loadChunk(coord);
            worldgen::generateChunkTerrain(*chunk, coord, fbm);
        }
    }

    // Cross-chunk meshing requires neighbors exist
    {
        ScopedTimer t{"meshing"};
        for (const ChunkCoord& coord : m_coords) {
            rebuildChunkMesh(coord);
        }
    }

    m_window.attachInput(&m_input);
    m_imgui.emplace(m_window);

    setupGlState();

    unsigned int sampleQuery = 0;
    glGenQueries(1, &sampleQuery);
    m_sampleQuery = QueryHandle{sampleQuery};

    glGetIntegerv(GL_SAMPLES, &m_actualSamples);
    if (m_actualSamples == 0) {
        m_actualSamples = 1;
    }

    m_lastFrameTime = glfwGetTime();
}

void Application::run() {
    bool firstFrame = true;
    while (!m_window.shouldClose()) {
        const auto now = glfwGetTime();
        auto dt = static_cast<float>(now - m_lastFrameTime);
        dt = std::min(dt, config::DT_CAP);
        m_lastFrameTime = now;

        update(dt);
        render();
        m_window.swapBuffers();
        if (firstFrame) {
            auto ms = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() -
                                                                m_appStart)
                          .count();
            spdlog::info("[profile] time to first frame: {:.1f} ms", ms);
            firstFrame = false;
        }
    }
}

void Application::update(float dt) {
    // Frame stats
    if (dt > 0.0F) {
        m_fpsAccumDt += dt;
        ++m_fpsFrameCount;
        if (m_fpsAccumDt >= FPS_SAMPLE_INTERVAL) {
            m_fps = static_cast<float>(m_fpsFrameCount) / m_fpsAccumDt;
            m_fpsAccumDt = 0.0F;
            m_fpsFrameCount = 0;
        }
    }

    // Input
    m_input.update(m_window.raw());
    m_window.pollEvents();
    m_input.setUiCapture(m_imgui->wantCaptureMouse(), m_imgui->wantCaptureKeyboard());
    handleSpecialKeys(m_input, m_overlayVisible, m_window, m_wireframe);
    handleBlockHotkeys(m_input, m_selectedBlock);

    // Simulation
    m_camera.update(m_input, dt);

    // Targeting
    m_lookingAt =
        raycast(m_chunkManager, m_camera.position(), m_camera.forward(), config::MAX_REACH);
    if (m_lookingAt && m_lookingAt->face) {
        if (!m_lastOutlineCell || *m_lastOutlineCell != m_lookingAt->cell) {
            auto outline = cubeOutlineVertices(m_lookingAt->cell, OUTLINE_COLOR);
            m_lineMesh.upload(std::span{outline});
            m_lastOutlineCell = m_lookingAt->cell;
            auto chunk = m_chunkManager.getChunk(ChunkManager::worldToChunk(
                m_lookingAt->cell.x, m_lookingAt->cell.y, m_lookingAt->cell.z));
            if (chunk) {
                m_targetBlockName = blockName(
                    chunk->getOrAir(m_lookingAt->cell.x, m_lookingAt->cell.y, m_lookingAt->cell.z));
            }
        }
    } else if (m_lastOutlineCell) {
        m_lineMesh.upload(std::span<const LineVertex>{});
        m_lastOutlineCell.reset();
    }

    // Block interaction
    handleBreakBlock(m_input, m_chunkManager, m_lookingAt);
    handlePlaceBlock(m_input, m_chunkManager, m_lookingAt, m_selectedBlock);
    handlePickBlock(m_input, m_chunkManager, m_lookingAt, m_selectedBlock);

    // Remesh Dirty Chunk
    for (const auto& coord : m_coords) {
        auto chunk = m_chunkManager.getChunk(coord);
        if (chunk && chunk->isDirty()) {
            rebuildChunkMesh(coord);
        }
    }
}

void Application::render() {
    // UI
    m_imgui->beginFrame();

    const HudInfo hud{
        .cameraPos = m_camera.position(),
        .selectedBlock = m_selectedBlock,
        .fps = m_fps,
    };

    const DebugInfo debug{
        .yaw = m_camera.yaw(),
        .pitch = m_camera.pitch(),
        .vertexCount = totalVertexCount(),
        .samplesPassed = m_samplesPassed,
        .actualSamples = m_actualSamples,
        .lookingAt = m_lookingAt,
        .targetBlockName = m_targetBlockName,
        .drawnChunks = m_drawnChunks,
        .totalChunks = static_cast<int>(m_chunkMesh.size()),
    };

    drawHud(hud, debug, m_overlayVisible);

    drawCrosshair();

    // Scene
    glBeginQuery(GL_SAMPLES_PASSED, m_sampleQuery.get());
    m_drawnChunks = renderScene(m_chunkShader,
                                m_chunkMesh,
                                m_camera,
                                m_blockTextures,
                                m_window.aspect(),
                                m_wireframe,
                                m_lineMesh,
                                m_lineShader);
    glEndQuery(GL_SAMPLES_PASSED);
    glGetQueryObjectui64v(m_sampleQuery.get(), GL_QUERY_RESULT, &m_samplesPassed);

    m_imgui->endFrame();
}

void Application::rebuildChunkMesh(ChunkCoord coord) {
    const std::shared_ptr<Chunk> center = m_chunkManager.getChunk(coord);
    if (center == nullptr) {
        throw std::runtime_error(
            fmt::format("Failed to load chunk at ({}, {}, {})", coord.x, coord.y, coord.z));
    }
    std::array<std::shared_ptr<Chunk>, mesher::FACE_COUNT> neighbors;
    for (std::size_t f = 0; f < mesher::FACE_COUNT; ++f) {
        neighbors[f] = m_chunkManager.getChunk(coord + faceNormal(Face(f)));
    }
    BlockAccessor accessor{center, neighbors};
    auto vertices = mesher::buildMesh(accessor);
    m_chunkMesh[coord].upload(vertices);
    center->clearDirty();
}

int Application::totalVertexCount() const {
    int total = 0;
    for (const auto& [coord, mesh] : m_chunkMesh) {
        total += mesh.vertexCount();
    }
    return total;
}

}  // namespace hs
