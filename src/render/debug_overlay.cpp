#include <hewnstead/render/camera.hpp>
#include <hewnstead/render/debug_overlay.hpp>
#include <hewnstead/world/raycast.hpp>

#include <glad/gl.h>

#include <glm/trigonometric.hpp>

#include <cmath>
#include <imgui.h>

namespace hs {

namespace {

// Small offset of HUD positioning
constexpr float HUD_PAD_X = 10.0F;
constexpr float HUD_PAD_Y = 10.0F;

// HUD font scale relative to ImGui default
constexpr float HUD_FONT_SCALE = 1.5F;

// Degrees in a full revolution (yaw wraps to [0, DEG_PER_REV))
constexpr float DEG_PER_REV = 360.0F;
constexpr int DEG_PER_REV_INT = 360;

// FPS update every 0.5 sec
constexpr float SAMPLE_INTERVAL = 0.5F;

}  // namespace

void drawCameraHud(const Camera& camera,
                   float dt,
                   const std::optional<RaycastHit>& lookingAt,
                   const char* targetBlockName,
                   BlockId selectedBlock) {
    constexpr ImGuiWindowFlags FLAGS = ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoDecoration |
                                       ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoNav |
                                       ImGuiWindowFlags_AlwaysAutoResize |
                                       ImGuiWindowFlags_NoSavedSettings;

    ImGui::SetNextWindowPos(ImVec2(HUD_PAD_X, HUD_PAD_Y), ImGuiCond_Always);
    ImGui::Begin("##CameraHud", nullptr, FLAGS);
    ImGui::SetWindowFontScale(HUD_FONT_SCALE);

    const glm::vec3 pos = camera.position();

    float yawDeg = glm::degrees(camera.yaw());
    yawDeg = std::fmod(yawDeg, DEG_PER_REV);
    if (yawDeg < 0.0F) {
        yawDeg += DEG_PER_REV;
    }
    const int yawInt = static_cast<int>(yawDeg) % DEG_PER_REV_INT;

    const int pitchInt = static_cast<int>(glm::degrees(camera.pitch()));

    static float displayedFps = 0.0F;
    static float accumDt = 0.0F;
    static int frameCount = 0;

    if (dt > 0.0F) {
        accumDt += dt;
        frameCount++;

        if (accumDt >= SAMPLE_INTERVAL) {
            displayedFps = static_cast<float>(frameCount) / accumDt;
            accumDt = 0.0F;
            frameCount = 0;
        }
    }

    ImGui::Text("pos: x: %.2f, y: %.2f, z: %.2f", pos.x, pos.y, pos.z);
    ImGui::Text("yaw: %d\xc2\xb0 pitch %d\xc2\xb0", yawInt, pitchInt);
    ImGui::Text("fps: %.0f", displayedFps);

    ImGui::Text("Block Selected: %s", blockName(selectedBlock));

    ImGui::Separator();
    ImGui::Text("Raycast:");
    if (lookingAt) {
        ImGui::Text(
            "  cell: (%d, %d, %d)", lookingAt->cell.x, lookingAt->cell.y, lookingAt->cell.z);
        if (lookingAt->face) {
            ImGui::Text("  face: %s", faceToString(*lookingAt->face));
            ImGui::Text("  distance: %.2f", lookingAt->distance);
            if (targetBlockName != nullptr) {
                ImGui::Text("  block: %s", targetBlockName);
            }
        } else {
            ImGui::Text("  (inside block)");
        }
    } else {
        ImGui::Text("  (none)");
    }

    ImGui::End();
}

}  // namespace hs
