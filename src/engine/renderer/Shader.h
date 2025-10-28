#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

class Shader {
public:
    GLuint programID;

    Shader() : programID(0) {}

    // Needed to load shaders from path (can be cached later)
    bool loadFromFiles(const char* vertexPath, const char* fragmentPath);
    void use() const;

    // setters for values
    void setMat4(const char* name, const glm::mat4& mat) const;
    void setVec3(const char* name, const glm::vec3& vec) const;
    void setFloat(const char* name, float value) const;
    void setInt(const char* name, int value) const;

private:
    static bool compileShader(const char* source, GLenum type, GLuint& shader);
    static std::string readFile(const char* filePath);
};