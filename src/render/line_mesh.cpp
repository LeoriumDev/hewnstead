#include <hewnstead/core/glcheck.hpp>
#include <hewnstead/render/line_mesh.hpp>

#include <glad/gl.h>

#include <cstddef>

namespace hs {

namespace {

constexpr GLuint POSITION_ATTRIB = 0;
constexpr GLuint COLOR_ATTRIB = 1;

}  // namespace

LineMesh::LineMesh() {
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

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

LineMesh::~LineMesh() {
    if (m_vao != 0) {
        glDeleteVertexArrays(1, &m_vao);
    }
    if (m_vbo != 0) {
        glDeleteBuffers(1, &m_vbo);
    }
}

LineMesh::LineMesh(LineMesh&& other) noexcept
    : m_vao(other.m_vao), m_vbo(other.m_vbo), m_vertexCount(other.m_vertexCount) {
    other.m_vao = 0;
    other.m_vbo = 0;
    other.m_vertexCount = 0;
}

LineMesh& LineMesh::operator=(LineMesh&& other) noexcept {
    if (this != &other) {
        if (m_vao != 0) {
            glDeleteVertexArrays(1, &m_vao);
        }
        if (m_vbo != 0) {
            glDeleteBuffers(1, &m_vbo);
        }
        m_vao = other.m_vao;
        m_vbo = other.m_vbo;
        m_vertexCount = other.m_vertexCount;
        other.m_vao = 0;
        other.m_vbo = 0;
        other.m_vertexCount = 0;
    }
    return *this;
}

void LineMesh::draw() const {
    if (m_vertexCount == 0) {
        return;
    }
    glBindVertexArray(m_vao);
    glDrawArrays(GL_LINES, 0, m_vertexCount);
    glBindVertexArray(0);
}

void LineMesh::upload(std::span<const LineVertex> vertices) {
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(vertices.size_bytes()),
                 vertices.data(),
                 GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    m_vertexCount = static_cast<int>(vertices.size());
}

}  // namespace hs
