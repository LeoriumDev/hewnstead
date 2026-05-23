#pragma once

#include <hewnstead/render/chunk_vertex.hpp>

#include <span>

namespace hs {

class ChunkMesh {
public:
    ChunkMesh();
    ~ChunkMesh();

    ChunkMesh(const ChunkMesh&) = delete;
    ChunkMesh& operator=(const ChunkMesh&) = delete;
    ChunkMesh(ChunkMesh&&) noexcept;
    ChunkMesh& operator=(ChunkMesh&&) noexcept;

    void draw() const;
    void upload(std::span<const ChunkVertex> vertices);

    [[nodiscard]] int vertexCount() const { return m_vertexCount; }

private:
    unsigned int m_vao = 0;
    unsigned int m_vbo = 0;
    int m_vertexCount = 0;
};

}  // namespace hs
