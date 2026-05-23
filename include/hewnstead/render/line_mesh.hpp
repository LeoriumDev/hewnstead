#pragma once

#include <hewnstead/render/line_vertex.hpp>

#include <span>

namespace hs {

class LineMesh {
public:
    LineMesh();
    ~LineMesh();

    LineMesh(const LineMesh&) = delete;
    LineMesh& operator=(const LineMesh&) = delete;
    LineMesh(LineMesh&&) noexcept;
    LineMesh& operator=(LineMesh&&) noexcept;

    void draw() const;
    void upload(std::span<const LineVertex> vertices);

    [[nodiscard]] int vertexCount() const { return m_vertexCount; }

private:
    unsigned int m_vao = 0;
    unsigned int m_vbo = 0;
    int m_vertexCount = 0;
};

}  // namespace hs
