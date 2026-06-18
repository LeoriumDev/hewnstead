#include <hewnstead/core/gl_check.hpp>
#include <hewnstead/render/line_mesh.hpp>

#include <glad/gl.h>

#include <cstddef>

namespace hs {

namespace {

constexpr GLuint POSITION_ATTRIB = 0;
constexpr GLuint COLOR_ATTRIB = 1;

}  // namespace

LineMesh::LineMesh() {
    unsigned int vao = 0;
    unsigned int vbo = 0;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    m_vao = VertexArrayHandle{vao};
    m_vbo = VertexBufferHandle{vbo};
    glBindVertexArray(m_vao.get());
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo.get());

    // NOLINTBEGIN(performance-no-int-to-ptr)

    glVertexAttribPointer(POSITION_ATTRIB,
                          glm::vec3::length(),
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(LineVertex),
                          reinterpret_cast<void*>(offsetof(LineVertex, position)));
    glEnableVertexAttribArray(POSITION_ATTRIB);

    glVertexAttribPointer(COLOR_ATTRIB,
                          glm::vec3::length(),
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(LineVertex),
                          reinterpret_cast<void*>(offsetof(LineVertex, color)));
    glEnableVertexAttribArray(COLOR_ATTRIB);

    // NOLINTEND(performance-no-int-to-ptr)

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    GL_CHECK();
}

void LineMesh::draw() const {
    if (m_vertexCount == 0) {
        return;
    }
    glBindVertexArray(m_vao.get());
    glDrawArrays(GL_LINES, 0, m_vertexCount);
    glBindVertexArray(0);
}

void LineMesh::upload(std::span<const LineVertex> vertices) {
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo.get());
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(vertices.size_bytes()),
                 vertices.data(),
                 GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    m_vertexCount = static_cast<int>(vertices.size());
}

}  // namespace hs
