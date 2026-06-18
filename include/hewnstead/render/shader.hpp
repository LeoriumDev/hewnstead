#pragma once

#include <hewnstead/core/gl_objects.hpp>

#include <glm/mat4x4.hpp>

#include <string>
#include <string_view>

namespace hs {

class Shader {
public:
    Shader(std::string_view vertPath, std::string_view fragPath);
    ~Shader() = default;

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    Shader(Shader&&) = delete;
    Shader& operator=(Shader&&) = delete;

    void use() const;

    void setMat4(const std::string& name, const glm::mat4& value) const;
    void setInt(const std::string& name, int value) const;

    [[nodiscard]] unsigned int id() const { return m_program.get(); }

private:
    ProgramHandle m_program;
};

}  // namespace hs
