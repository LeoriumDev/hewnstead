#pragma once

#include <hewnstead/world/block_id.hpp>

#include <optional>

namespace hs {

class Camera;
struct RaycastHit;

// Draw the always-on camera HUD in the top-left corner. Call between
// ImguiRuntime::beginFrame() and the scene render in the main loop.
void drawCameraHud(const Camera& camera,
                   float dt,
                   const std::optional<RaycastHit>& lookingAt,
                   const char* targetBlockName,
                   BlockId selectedBlock);

}  // namespace hs
