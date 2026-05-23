#pragma once

#include <hewnstead/world/block_id.hpp>
#include <hewnstead/world/raycast.hpp>

#include <glad/gl.h>

#include <glm/vec3.hpp>

#include <optional>

namespace hs {

// Always-on section
struct HudInfo {
    glm::vec3 cameraPos;
    BlockId selectedBlock;
    float fps;
};

// F3-only section
struct DebugInfo {
    float yaw;    // radians
    float pitch;  // radians
    int vertexCount;
    GLuint64 samplesPassed;
    GLint actualSamples;
    std::optional<RaycastHit> lookingAt;
    const char* targetBlockName;
};

void drawHud(const HudInfo& info, const DebugInfo& debug, bool showDebug);

}  // namespace hs
