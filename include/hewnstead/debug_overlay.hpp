#pragma once

namespace hs {

class Camera;

// Draw the always-on camera HUD in the top-left corner. Call between
// ImguiRuntime::beginFrame() and the scene render in the main loop.
void drawCameraHud(const Camera& camera, float dt);

}  // namespace hs
