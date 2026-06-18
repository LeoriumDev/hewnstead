#pragma once

#include <hewnstead/core/input.hpp>
#include <hewnstead/core/window.hpp>
#include <hewnstead/render/camera.hpp>
#include <hewnstead/render/chunk_mesh.hpp>
#include <hewnstead/render/imgui_runtime.hpp>
#include <hewnstead/render/line_mesh.hpp>
#include <hewnstead/render/shader.hpp>
#include <hewnstead/render/texture_array.hpp>
#include <hewnstead/world/block_id.hpp>
#include <hewnstead/world/chunk_coord.hpp>
#include <hewnstead/world/chunk_manager.hpp>
#include <hewnstead/world/raycast.hpp>

#include <glad/gl.h>

#include <glm/ext/vector_int3.hpp>

#include <memory>
#include <optional>
#include <unordered_map>

namespace hs {

class Chunk;

class Application {
public:
    Application();
    ~Application() = default;

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(Application&&) = delete;

    void run();

private:
    void update(float dt);
    void render();
    [[nodiscard]] int totalVertexCount() const;

    // Declaration order IS construction order.
    // Window first: it creates the GL context every later GL-touching member depends on.
    Window m_window;
    Shader m_chunkShader;
    Shader m_lineShader;
    TextureArray m_blockTextures;
    ChunkManager m_chunkManager;
    Input m_input;
    Camera m_camera;

    // Emplaced post-attachInput so ImGui chains our GLFW callbacks
    std::optional<ImguiRuntime> m_imgui;

    // GPU objects
    std::unordered_map<ChunkCoord, ChunkMesh> m_chunkMesh;
    LineMesh m_lineMesh;

    void rebuildChunkMesh(ChunkCoord coord);

    // Per-frame view of what the camera is pointed at
    std::optional<RaycastHit> m_lookingAt;

    // Loop state
    bool m_overlayVisible = false;
    bool m_wireframe = false;
    BlockId m_selectedBlock = blocks::Stone;
    const char* m_targetBlockName = nullptr;
    double m_lastFrameTime = 0.0;
    std::optional<glm::ivec3> m_lastOutlineCell;

    // Frame stats
    float m_fps = 0.0F;
    float m_fpsAccumDt = 0.0F;
    int m_fpsFrameCount = 0;

    // MSAA sample query
    QueryHandle m_sampleQuery;
    GLuint64 m_samplesPassed = 0;
    GLint m_actualSamples = 1;

    std::vector<ChunkCoord> m_coords;
};

}  // namespace hs
