#include <hewnstead/render/debug_overlay.hpp>

#include <glm/trigonometric.hpp>
#include <imgui.h>

#include <cmath>

namespace hs {

namespace {

constexpr float HUD_PAD_X = 10.0F;
constexpr float HUD_PAD_Y = 10.0F;
constexpr float HUD_FONT_SCALE = 1.5F;
constexpr float DEG_PER_REV = 360.0F;
constexpr int DEG_PER_REV_INT = 360;

}  // namespace

void drawHud(const HudInfo& info, const DebugInfo& debug, bool showDebug) {
    constexpr ImGuiWindowFlags FLAGS = ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoDecoration |
                                       ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoNav |
                                       ImGuiWindowFlags_AlwaysAutoResize |
                                       ImGuiWindowFlags_NoSavedSettings;

    ImGui::SetNextWindowPos(ImVec2(HUD_PAD_X, HUD_PAD_Y), ImGuiCond_Always);
    ImGui::Begin("##Hud", nullptr, FLAGS);
    ImGui::SetWindowFontScale(HUD_FONT_SCALE);

    // Always-on
    ImGui::Text(
        "pos: x: %.2f, y: %.2f, z: %.2f", info.cameraPos.x, info.cameraPos.y, info.cameraPos.z);
    ImGui::Text("fps: %.0f", info.fps);
    ImGui::Text("block: %s", blockName(info.selectedBlock));

    // F3-only
    if (showDebug) {
        // ImGui::Spacing();

        float yawDeg = glm::degrees(debug.yaw);
        yawDeg = std::fmod(yawDeg, DEG_PER_REV);
        if (yawDeg < 0.0F) {
            yawDeg += DEG_PER_REV;
        }
        const int yawInt = static_cast<int>(yawDeg) % DEG_PER_REV_INT;
        const int pitchInt = static_cast<int>(glm::degrees(debug.pitch));

        ImGui::Text("yaw: %d\xc2\xb0 pitch: %d\xc2\xb0", yawInt, pitchInt);
        ImGui::Text("vertices: %d", debug.vertexCount);
        ImGui::Text("triangles: %d", debug.vertexCount / 3);
        ImGui::Text("samples: %llu (~%llu px @ MSAA %dx)",
                    debug.samplesPassed,
                    debug.samplesPassed / static_cast<GLuint64>(debug.actualSamples),
                    debug.actualSamples);
        ImGui::Text("chunks: %d/%d", debug.drawnChunks, debug.totalChunks);
        ImGui::Text("raycast:");
        if (debug.lookingAt) {
            ImGui::Text("  cell: (%d, %d, %d)",
                        debug.lookingAt->cell.x,
                        debug.lookingAt->cell.y,
                        debug.lookingAt->cell.z);
            if (debug.lookingAt->face) {
                ImGui::Text("  face: %s", faceToString(*debug.lookingAt->face));
                ImGui::Text("  distance: %.2f", debug.lookingAt->distance);
                if (debug.targetBlockName != nullptr) {
                    ImGui::Text("  block: %s", debug.targetBlockName);
                }
            } else {
                ImGui::Text("  (inside block)");
            }
        } else {
            ImGui::Text("  (none)");
        }
    }

    ImGui::End();
}

}  // namespace hs
