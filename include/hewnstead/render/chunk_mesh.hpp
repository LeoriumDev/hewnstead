#pragma once

#include "hewnstead/core/gl_objects.hpp"
#include <hewnstead/render/chunk_vertex.hpp>

#include <span>

namespace hs {

class ChunkMesh {
public:
    ChunkMesh();
    ~ChunkMesh() = default;

    ChunkMesh(const ChunkMesh&) = delete;
    ChunkMesh& operator=(const ChunkMesh&) = delete;
    ChunkMesh(ChunkMesh&&) noexcept = default;
    ChunkMesh& operator=(ChunkMesh&&) noexcept = default;

    void draw() const;
    void upload(std::span<const ChunkVertex> vertices);

    [[nodiscard]] int vertexCount() const { return m_vertexCount; }

private:
    VertexArrayHandle m_vao;
    VertexBufferHandle m_vbo;
    int m_vertexCount = 0;
};

}  // namespace hs
