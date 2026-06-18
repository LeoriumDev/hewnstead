#pragma once

#include <string_view>

namespace hs::config {

constexpr std::string_view TITLE = "Hewnstead";

constexpr std::string_view CHUNK_VERTEX_SHADER_PATH = "assets/shaders/chunk.vert";
constexpr std::string_view CHUNK_FRAGMENT_SHADER_PATH = "assets/shaders/chunk.frag";
constexpr std::string_view LINE_VERTEX_SHADER_PATH = "assets/shaders/line.vert";
constexpr std::string_view LINE_FRAGMENT_SHADER_PATH = "assets/shaders/line.frag";

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
constexpr float MAX_REACH = 5.0F;

// Cube outline
constexpr int CUBE_EDGE_COUNT = 12;
constexpr int CUBE_OUTLINE_VERTEX_COUNT = CUBE_EDGE_COUNT * 2;

}  // namespace hs::config
