#include <hewnstead/glcheck.hpp>
#include <hewnstead/shader.hpp>

#include <glad/gl.h>

#include <glm/gtc/type_ptr.hpp>
#include <spdlog/spdlog.h>

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace hs {

namespace {

std::string readFile(std::string_view path) {
    std::string pathStr(path);
    std::ifstream file{pathStr};
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open shader file: " + pathStr);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

class ShaderObject {
public:
    explicit ShaderObject(GLuint id) : m_id(id) {}
    ~ShaderObject() {
        if (m_id != 0) {
            glDeleteShader(m_id);
        }
    }

    ShaderObject(const ShaderObject&) = delete;
    ShaderObject& operator=(const ShaderObject&) = delete;
    ShaderObject(ShaderObject&&) = delete;
    ShaderObject& operator=(ShaderObject&&) = delete;

    [[nodiscard]] GLuint id() const { return m_id; }

private:
    GLuint m_id;
};

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
GLuint compileShader(GLenum type, std::string_view source, std::string_view label) {
    GLuint shader = glCreateShader(type);
    const char* src = source.data();
    const auto len = static_cast<GLint>(source.size());
    glShaderSource(shader, 1, &src, &len);
    glCompileShader(shader);

    GLint ok = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (ok == GL_FALSE) {
        GLint logLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
        std::vector<char> log(static_cast<size_t>(logLen));
        glGetShaderInfoLog(shader, logLen, nullptr, log.data());
        glDeleteShader(shader);
        throw std::runtime_error("Shader compile failed (" + std::string(label) +
                                 "): " + log.data());
    }
    return shader;
}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
GLuint linkProgram(GLuint vert, GLuint frag) {
    GLuint program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);

    GLint ok = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &ok);
    if (ok == GL_FALSE) {
        GLint logLen = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLen);
        std::vector<char> log(static_cast<size_t>(logLen));
        glGetProgramInfoLog(program, logLen, nullptr, log.data());
        glDeleteProgram(program);
        throw std::runtime_error(std::string("Program link failed: ") + log.data());
    }

    glDetachShader(program, vert);
    glDetachShader(program, frag);
    return program;
}

}  // namespace

Shader::Shader(std::string_view vertPath, std::string_view fragPath) {
    const std::string vertSource = readFile(vertPath);
    const std::string fragSource = readFile(fragPath);

    ShaderObject vert(compileShader(GL_VERTEX_SHADER, vertSource, vertPath));
    ShaderObject frag(compileShader(GL_FRAGMENT_SHADER, fragSource, fragPath));

    m_program = linkProgram(vert.id(), frag.id());

    spdlog::info("Shader linked: {} + {}", vertPath, fragPath);
}

Shader::~Shader() {
    if (m_program != 0) {
        glDeleteProgram(m_program);
    }
}

void Shader::use() const {
    glUseProgram(m_program);
}

void Shader::setMat4(const std::string& name, const glm::mat4& value) const {
    GLint location = glGetUniformLocation(m_program, name.c_str());
    if (location < 0) {
        spdlog::warn("Uniform '{}' not found in program {}", name, m_program);
        return;
    }
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    GL_CHECK();
}

void Shader::setInt(const std::string& name, int value) const {
    GLint location = glGetUniformLocation(m_program, name.c_str());

    if (location == -1) {
        spdlog::warn("Shader uniform '{}' not found or unused", name);
    }

    glUniform1i(location, value);
    GL_CHECK();
}

}  // namespace hs
