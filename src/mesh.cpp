#include <hewnstead/glcheck.hpp>
#include <hewnstead/mesh.hpp>

#include <glad/gl.h>

#include <array>
#include <stdexcept>

namespace hs {

Mesh::Mesh(std::span<const float> vertices, int floatsPerVertex) {
    if (floatsPerVertex <= 0) {
        throw std::runtime_error("Mesh: floatsPerVertex must be positive");
    }
    if (vertices.size() % static_cast<size_t>(floatsPerVertex) != 0) {
        throw std::runtime_error("Mesh: vertex data size not divisible by floatsPerVertex");
    }

    m_vertexCount = static_cast<int>(vertices.size() / static_cast<size_t>(floatsPerVertex));

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(vertices.size_bytes()),
                 vertices.data(),
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0,                // attribute slot
                          floatsPerVertex,  // components per vertex
                          GL_FLOAT,         // type
                          GL_FALSE,         // normalize?
                          floatsPerVertex * static_cast<GLsizei>(sizeof(float)),  // stride
                          nullptr);                                               // offset (= 0)
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    GL_CHECK();
}

Mesh::~Mesh() {
    if (m_vao != 0) {
        glDeleteVertexArrays(1, &m_vao);
    }
    if (m_vbo != 0) {
        glDeleteBuffers(1, &m_vbo);
    }
}

void Mesh::draw() const {
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);
    glBindVertexArray(0);
}

Mesh Mesh::triangle() {
    constexpr std::array verts = {
        -0.5F,
        -0.5F,
        0.0F,
        0.5F,
        -0.5F,
        0.0F,
        0.0F,
        0.5F,
        0.0F,
    };
    return Mesh{std::span<const float>(verts)};
}

}  // namespace hs
