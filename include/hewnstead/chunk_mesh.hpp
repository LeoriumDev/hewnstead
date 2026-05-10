#pragma once

#include <hewnstead/chunk_vertex.hpp>

#include <span>

namespace hs {

class ChunkMesh {
public:
    explicit ChunkMesh(std::span<const ChunkVertex> vertices);
    ~ChunkMesh();

    ChunkMesh(const ChunkMesh&) = delete;
    ChunkMesh& operator=(const ChunkMesh&) = delete;
    ChunkMesh(ChunkMesh&&) = delete;
    ChunkMesh& operator=(ChunkMesh&&) = delete;

    void draw() const;

    [[nodiscard]] int vertexCount() const { return m_vertexCount; }

private:
    unsigned int m_vao = 0;
    unsigned int m_vbo = 0;
    int m_vertexCount = 0;
};

}  // namespace hs
