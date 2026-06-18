#pragma once

#include <hewnstead/core/gl_objects.hpp>

#include <glad/gl.h>

#include <span>
#include <string_view>

namespace hs {

class TextureArray {
public:
    explicit TextureArray(std::span<const std::string_view> paths);
    ~TextureArray() = default;

    TextureArray(const TextureArray&) = delete;
    TextureArray& operator=(const TextureArray&) = delete;
    TextureArray(TextureArray&& other) noexcept = default;
    TextureArray& operator=(TextureArray&& other) noexcept = default;

    void bind(GLuint unit = 0) const;

    [[nodiscard]] GLuint id() const { return m_id.get(); }

    [[nodiscard]] GLsizei layerCount() const { return m_layerCount; }

private:
    TextureHandle m_id;
    GLsizei m_layerCount = 0;
};

}  // namespace hs
