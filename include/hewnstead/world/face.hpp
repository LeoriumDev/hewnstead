#pragma once

#include <glm/vec3.hpp>

#include <cstdint>

namespace hs {

enum class Face : std::uint8_t { East, West, Top, Bottom, South, North, FACE_COUNT };

[[nodiscard]] inline const char* faceToString(Face f) {
    switch (f) {
    case Face::East:
        return "East";
    case Face::West:
        return "West";
    case Face::Top:
        return "Top";
    case Face::Bottom:
        return "Bottom";
    case Face::South:
        return "South";
    case Face::North:
        return "North";
    default:
        return "?";
    }
}

[[nodiscard]] inline glm::ivec3 faceNormal(Face f) {
    switch (f) {
    case Face::East:
        return {1, 0, 0};
    case Face::West:
        return {-1, 0, 0};
    case Face::Top:
        return {0, 1, 0};
    case Face::Bottom:
        return {0, -1, 0};
    case Face::South:
        return {0, 0, 1};
    case Face::North:
        return {0, 0, -1};
    default:
        return {0, 0, 0};
    }
}
}  // namespace hs
