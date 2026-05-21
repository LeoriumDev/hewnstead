#pragma once

namespace hs::config {

// Window
constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 720;

// Clear color (dark blue-grey)
constexpr float CLEAR_R = 0.10F;
constexpr float CLEAR_G = 0.10F;
constexpr float CLEAR_B = 0.12F;
constexpr float CLEAR_A = 1.00F;

// Frame timing
constexpr float DT_CAP = 0.1F;

// Camera projection
constexpr float FOV_DEGREES = 60.0F;
constexpr float NEAR_PLANE = 0.1F;
constexpr float FAR_PLANE = 1000.0F;

// Block place/break reach
constexpr int MAX_REACH = 5;

// Cube outline
constexpr int CUBE_EDGES = 24;

}  // namespace hs::config
