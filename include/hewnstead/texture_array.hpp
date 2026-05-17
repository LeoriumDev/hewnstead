#pragma once

#include <glad/gl.h>

#include <span>
#include <string_view>

namespace hs {

class TextureArray {
public:
    explicit TextureArray(std::span<const std::string_view> paths);
    ~TextureArray();

    TextureArray(const TextureArray&) = delete;
    TextureArray& operator=(const TextureArray&) = delete;
    TextureArray(TextureArray&& other) noexcept;
    TextureArray& operator=(TextureArray&& other) noexcept;

    void bind(GLuint unit = 0) const;

    [[nodiscard]] GLuint id() const { return m_id; }
    [[nodiscard]] GLsizei layerCount() const { return m_layerCount; }

private:
    GLuint m_id = 0;
    GLsizei m_layerCount = 0;
};

}  // namespace hs
