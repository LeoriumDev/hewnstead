#include "hewnstead/glcheck.hpp"

#include <glad/gl.h>

#include <spdlog/spdlog.h>

namespace hs {

void glCheckError(std::string_view file, int line) {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        const char* name;
        switch (err) {
        case GL_INVALID_ENUM:
            name = "INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            name = "INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            name = "INVALID_OPERATION";
            break;
        case GL_OUT_OF_MEMORY:
            name = "OUT_OF_MEMORY";
            break;
        default:
            name = "UNKNOWN";
            break;
        }
        spdlog::error("GL error {} ({:#x}) at {}:{}", name, err, file, line);
    }
}

}  // namespace hs
