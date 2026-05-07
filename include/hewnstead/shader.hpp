#pragma once

#include <glm/mat4x4.hpp>

#include <string_view>

namespace hs {

class Shader {
public:
    Shader(std::string_view vertPath, std::string_view fragPath);
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    Shader(Shader&&) = delete;
    Shader& operator=(Shader&&) = delete;

    void use() const;

    void setMat4(const std::string& name, const glm::mat4& value) const;

    [[nodiscard]] unsigned int id() const { return m_program; }

private:
    unsigned int m_program = 0;  // OpenGL's invalid handle
};

}  // namespace hs
