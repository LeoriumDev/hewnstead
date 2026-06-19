#include <hewnstead/core/gl_check.hpp>
#include <hewnstead/render/texture_array.hpp>

#include <spdlog/spdlog.h>
#include <stb_image.h>

#include <cassert>
#include <cmath>
#include <stdexcept>

namespace hs {

namespace {

constexpr GLsizei TEXTURE_WIDTH = 32;
constexpr GLsizei TEXTURE_HEIGHT = 32;

void uploadLayer(std::string_view path, GLint layer) {
    int loadedW = 0;
    int loadedH = 0;
    int loadedC = 0;
    const std::string path_str{path};
    unsigned char* pixels =
        stbi_load(path_str.c_str(), &loadedW, &loadedH, &loadedC, STBI_rgb_alpha);

    if (pixels == nullptr) {
        const char* reason = stbi_failure_reason();
        throw std::runtime_error("Failed to load texture '" + path_str +
                                 "': " + ((reason != nullptr) ? reason : "unknown"));
    }
    if (loadedW != TEXTURE_WIDTH || loadedH != TEXTURE_HEIGHT) {
        stbi_image_free(pixels);
        throw std::runtime_error("Texture '" + path_str + "' has wrong dimensions: expected" +
                                 std::to_string(TEXTURE_WIDTH) + "x" +
                                 std::to_string(TEXTURE_HEIGHT));
    }
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
                    0,
                    0,
                    0,
                    layer,
                    TEXTURE_WIDTH,
                    TEXTURE_HEIGHT,
                    1,
                    GL_RGBA,
                    GL_UNSIGNED_BYTE,
                    pixels);
    stbi_image_free(pixels);
}

}  // namespace

TextureArray::TextureArray(std::span<const std::string_view> paths) {
    assert(!paths.empty() && "TextureArray needs at least one texture");
    const GLsizei mipLevels = 1 + static_cast<GLsizei>(std::floor(std::log2(TEXTURE_WIDTH)));
    m_layerCount = static_cast<GLsizei>(paths.size());
    unsigned int id = 0;
    glGenTextures(1, &id);
    m_id = TextureHandle{id};
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_id.get());
    glTexStorage3D(GL_TEXTURE_2D_ARRAY,
                   mipLevels,
                   GL_SRGB8_ALPHA8,
                   TEXTURE_WIDTH,
                   TEXTURE_HEIGHT,
                   m_layerCount);

    stbi_set_flip_vertically_on_load(1);

    for (GLint layer = 0; layer < m_layerCount; ++layer) {
        uploadLayer(paths[static_cast<std::size_t>(layer)], layer);
    }

    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    GL_CHECK();

    spdlog::info("TextureArray: loaded {} layers, {}x{} GL_SRGB8_ALPHA8",
                 m_layerCount,
                 TEXTURE_WIDTH,
                 TEXTURE_HEIGHT);
}

void TextureArray::bind(GLuint unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_id.get());
}

}  // namespace hs
