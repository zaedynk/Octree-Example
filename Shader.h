#pragma once

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

class Shader {
public:
    GLuint program;

    Shader() : program(0) {} // Default constructor

    Shader(const std::string& vertexPath, const std::string& fragmentPath); // Existing constructor

    ~Shader();

    void use() const;

    void setMat4(const std::string& name, const glm::mat4& mat) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec4(const std::string& name, glm::vec4 value);
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;

private:
    void checkCompileErrors(GLuint shader, const std::string& type) const;
};
