#include <hewnstead/glcheck.hpp>
#include <hewnstead/texture_array.hpp>

#include <spdlog/spdlog.h>
#include <stb_image.h>

#include <cassert>
#include <stdexcept>

namespace hs {

namespace {

constexpr GLsizei TEXTURE_WIDTH = 32;
constexpr GLsizei TEXTURE_HEIGHT = 32;
constexpr GLint MIP_LEVELS = 1;

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

    m_layerCount = static_cast<GLsizei>(paths.size());

    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_id);
    glTexStorage3D(
        GL_TEXTURE_2D_ARRAY, MIP_LEVELS, GL_RGBA8, TEXTURE_WIDTH, TEXTURE_HEIGHT, m_layerCount);

    stbi_set_flip_vertically_on_load(1);

    for (GLint layer = 0; layer < m_layerCount; ++layer) {
        uploadLayer(paths[static_cast<std::size_t>(layer)], layer);
    }

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    GL_CHECK();

    spdlog::info("TextureArray: loaded {} layers, {}x{} GL_RGBA8",
                 m_layerCount,
                 TEXTURE_WIDTH,
                 TEXTURE_HEIGHT);
}

TextureArray::~TextureArray() {
    if (m_id != 0) {
        glDeleteTextures(1, &m_id);
    }
}

TextureArray::TextureArray(TextureArray&& other) noexcept
    : m_id(other.m_id), m_layerCount(other.m_layerCount) {
    other.m_id = 0;
    other.m_layerCount = 0;
}

TextureArray& TextureArray::operator=(TextureArray&& other) noexcept {
    if (this != &other) {
        if (m_id != 0) {
            glDeleteTextures(1, &m_id);
        }
        m_id = other.m_id;
        m_layerCount = other.m_layerCount;
        other.m_id = 0;
        other.m_layerCount = 0;
    }
    return *this;
}

void TextureArray::bind(GLuint unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_id);
}

}  // namespace hs
