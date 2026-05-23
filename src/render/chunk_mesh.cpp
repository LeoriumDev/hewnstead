#include <hewnstead/core/glcheck.hpp>
#include <hewnstead/render/chunk_mesh.hpp>

#include <glad/gl.h>

#include <cstddef>  // offsetof

namespace hs {

namespace {

constexpr GLuint POSITION_ATTRIB = 0;
constexpr GLuint UV_ATTRIB = 1;
constexpr GLuint LAYER_ATTRIB = 2;

}  // namespace

ChunkMesh::ChunkMesh() {
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

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

    // Attribute 1: texture coord (vec2, offset 12)
    glVertexAttribPointer(UV_ATTRIB,
                          glm::vec2::length(),
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(ChunkVertex),
                          reinterpret_cast<void*>(offsetof(ChunkVertex, uv)));
    glEnableVertexAttribArray(UV_ATTRIB);

    // Attribute 2: texture array layer index (float, offset 20)
    glVertexAttribPointer(LAYER_ATTRIB,
                          1,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(ChunkVertex),
                          reinterpret_cast<void*>(offsetof(ChunkVertex, layer)));
    glEnableVertexAttribArray(LAYER_ATTRIB);

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

ChunkMesh::ChunkMesh(ChunkMesh&& other) noexcept
    : m_vao(other.m_vao), m_vbo(other.m_vbo), m_vertexCount(other.m_vertexCount) {
    other.m_vao = 0;
    other.m_vbo = 0;
    other.m_vertexCount = 0;
}

ChunkMesh& ChunkMesh::operator=(ChunkMesh&& other) noexcept {
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

void ChunkMesh::draw() const {
    if (m_vertexCount == 0) {
        return;
    }
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);
    glBindVertexArray(0);
}

void ChunkMesh::upload(std::span<const ChunkVertex> vertices) {
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(vertices.size_bytes()),
                 vertices.data(),
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_vertexCount = static_cast<int>(vertices.size());
}

}  // namespace hs
