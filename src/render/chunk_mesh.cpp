#include <hewnstead/core/gl_check.hpp>
#include <hewnstead/core/gl_handle.hpp>
#include <hewnstead/core/gl_objects.hpp>
#include <hewnstead/render/chunk_mesh.hpp>

#include <glad/gl.h>

#include <cstddef>  // offsetof

namespace hs {

namespace {

constexpr GLuint POSITION_ATTRIB = 0;
constexpr GLuint UV_ATTRIB = 1;
constexpr GLuint LAYER_ATTRIB = 2;
constexpr GLuint SHADE_ATTRIB = 3;

}  // namespace

ChunkMesh::ChunkMesh() {
    unsigned int vao = 0;
    unsigned int vbo = 0;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    m_vao = VertexArrayHandle{vao};
    m_vbo = VertexBufferHandle{vbo};

    glBindVertexArray(m_vao.get());
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo.get());

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

    // Attribute 3: directional shading
    glVertexAttribPointer(SHADE_ATTRIB,
                          1,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(ChunkVertex),
                          reinterpret_cast<void*>(offsetof(ChunkVertex, shade)));
    glEnableVertexAttribArray(SHADE_ATTRIB);

    // NOLINTEND(performance-no-int-to-ptr)

    // Unbind to prevent later GL calls from accidentally modifying this VAO/VBO.
    // 0 is OpenGL's reserved sentinel for "no current binding".
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    GL_CHECK();
}

void ChunkMesh::draw() const {
    if (m_vertexCount == 0) {
        return;
    }
    glBindVertexArray(m_vao.get());
    glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);
    glBindVertexArray(0);
}

void ChunkMesh::upload(std::span<const ChunkVertex> vertices) {
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo.get());
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(vertices.size_bytes()),
                 vertices.data(),
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_vertexCount = static_cast<int>(vertices.size());
}

}  // namespace hs
