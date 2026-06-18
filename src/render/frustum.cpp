#include <hewnstead/render/frustum.hpp>

#include <algorithm>

namespace hs {

Frustum::Frustum(const glm::mat4& viewProjMatrix) {
    glm::mat4 matrix = glm::transpose(viewProjMatrix);
    m_planes = {
        matrix[3] + matrix[0],  // left
        matrix[3] - matrix[0],  // right
        matrix[3] + matrix[1],  // bottom
        matrix[3] - matrix[1],  // top
        matrix[3] + matrix[2],  // near
        matrix[3] - matrix[2],  // far
    };
}

bool Frustum::isVisible(const glm::vec3& min, const glm::vec3& max) const {
    return std::ranges::all_of(m_planes, [&](const glm::vec4& plane) -> bool {
        glm::vec3 p = {
            (plane.x > 0 ? max.x : min.x),
            (plane.y > 0 ? max.y : min.y),
            (plane.z > 0 ? max.z : min.z),
        };
        return (glm::dot(p, glm::vec3(plane)) + plane.w) >= 0;
    });
}

}  // namespace hs
