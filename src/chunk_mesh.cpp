#include <hewnstead/chunk_mesh.hpp>
#include <hewnstead/glcheck.hpp>

#include <glad/gl.h>

#include <cstddef>  // offsetof

namespace hs {

namespace {

constexpr GLuint POSITION_ATTRIB = 0;
constexpr GLuint COLOR_ATTRIB = 1;

}  // namespace

ChunkMesh::ChunkMesh(std::span<const ChunkVertex> vertices)
    : m_vertexCount(static_cast<int>(vertices.size())) {
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(vertices.size_bytes()),
                 vertices.data(),
                 GL_STATIC_DRAW);

    // OpenGL legacy: glVertexAttribPointer's last param is byte offset
    // disguised as void* due to pre-VBO API signature

    // NOLINTBEGIN(performance-no-int-to-ptr)

    // Attribute 0: position (vec3, offset 0)
    glVertexAttribPointer(POSITION_ATTRIB,
                          glm::vec3::length(),
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(ChunkVertex),
                          reinterpret_cast<void*>(offsetof(ChunkVertex, position)));
    glEnableVertexAttribArray(POSITION_ATTRIB);

    // Attribute 1: color (vec3, offset 12)
    glVertexAttribPointer(COLOR_ATTRIB,
                          glm::vec3::length(),
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(ChunkVertex),
                          reinterpret_cast<void*>(offsetof(ChunkVertex, color)));
    glEnableVertexAttribArray(COLOR_ATTRIB);

    // NOLINTEND(performance-no-int-to-ptr)

    // Unbind to prevent later GL calls from accidentally modifying this VAO/VBO.
    // 0 is OpenGL's reserved sentinel for "no current binding".
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    GL_CHECK();
}

ChunkMesh::~ChunkMesh() {
    if (m_vao != 0) {
        glDeleteVertexArrays(1, &m_vao);
    }
    if (m_vbo != 0) {
        glDeleteBuffers(1, &m_vbo);
    }
}

void ChunkMesh::draw() const {
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);
    glBindVertexArray(0);
}

}  // namespace hs
