#pragma once

#include <span>

namespace hs {

class Mesh {
public:
    explicit Mesh(std::span<const float> vertices, int floatsPerVertex = 3);
    ~Mesh();

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh(Mesh&&) = delete;
    Mesh& operator=(Mesh&&) = delete;

    void draw() const;

    static Mesh triangle();

private:
    unsigned int m_vao = 0;
    unsigned int m_vbo = 0;
    int m_vertexCount = 0;
};

}  // namespace hs
