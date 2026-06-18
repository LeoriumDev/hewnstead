#pragma once

#include <glm/glm.hpp>

#include <array>

namespace hs {

class Frustum {
public:
    explicit Frustum(const glm::mat4& viewProjMatrix);

    // True if the world-space AABB [min, max] is at least partially inside.
    [[nodiscard]] bool isVisible(const glm::vec3& min, const glm::vec3& max) const;

private:
    static constexpr std::size_t PLANE_COUNT = 6;
    std::array<glm::vec4, PLANE_COUNT> m_planes{};  // (a,b,c,d), inward normals, unnormalized
};

}  // namespace hs
