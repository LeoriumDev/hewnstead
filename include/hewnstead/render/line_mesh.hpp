#pragma once

#include <hewnstead/core/gl_objects.hpp>
#include <hewnstead/render/line_vertex.hpp>

#include <span>

namespace hs {

class LineMesh {
public:
    LineMesh();
    ~LineMesh() = default;

    LineMesh(const LineMesh&) = delete;
    LineMesh& operator=(const LineMesh&) = delete;
    LineMesh(LineMesh&&) noexcept = default;
    LineMesh& operator=(LineMesh&&) noexcept = default;

    void draw() const;
    void upload(std::span<const LineVertex> vertices);

    [[nodiscard]] int vertexCount() const { return m_vertexCount; }

private:
    VertexArrayHandle m_vao;
    VertexBufferHandle m_vbo;
    int m_vertexCount = 0;
};

}  // namespace hs
