#pragma once

#include <glm/vec3.hpp>

#include <cstdint>

namespace hs {

enum Face : std::uint8_t { East, West, Top, Bottom, South, North, FACE_COUNT };

[[nodiscard]] inline const char* faceToString(Face f) {
    switch (f) {
    case East:
        return "East";
    case West:
        return "West";
    case Top:
        return "Top";
    case Bottom:
        return "Bottom";
    case South:
        return "South";
    case North:
        return "North";
    default:
        return "?";
    }
}

[[nodiscard]] inline glm::ivec3 faceNormal(Face f) {
    switch (f) {
    case East:
        return {1, 0, 0};
    case West:
        return {-1, 0, 0};
    case Top:
        return {0, 1, 0};
    case Bottom:
        return {0, -1, 0};
    case South:
        return {0, 0, 1};
    case North:
        return {0, 0, -1};
    default:
        return {0, 0, 0};
    }
}
}  // namespace hs
