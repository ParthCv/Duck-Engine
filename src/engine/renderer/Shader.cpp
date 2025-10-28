#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>

std::string Shader::readFile(const char* filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool Shader::compileShader(const char* source, GLenum type, GLuint& shader) {
    shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    // Check compilation
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation failed:\n" << infoLog << std::endl;
        return false;
    }

    return true;
}

bool Shader::loadFromFiles(const char* vertexPath, const char* fragmentPath) {
    // Read shader files
    const std::string vertexCode = readFile(vertexPath);
    const std::string fragmentCode = readFile(fragmentPath);

    if (vertexCode.empty() || fragmentCode.empty()) {
        return false;
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // Compile shaders
    GLuint vertex, fragment;
    if (!compileShader(vShaderCode, GL_VERTEX_SHADER, vertex)) {
        return false;
    }
    if (!compileShader(fShaderCode, GL_FRAGMENT_SHADER, fragment)) {
        glDeleteShader(vertex);
        return false;
    }

    // Link to program
    programID = glCreateProgram();
    glAttachShader(programID, vertex);
    glAttachShader(programID, fragment);
    glLinkProgram(programID);

    // Check linking
    int success;
    char infoLog[512];
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(programID, 512, nullptr, infoLog);
        std::cerr << "Shader linking failed:\n" << infoLog << std::endl;
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        return false;
    }

    // Cleanup
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    std::cout << "Shader loaded successfully" << std::endl;
    return true;
}

void Shader::use() const {
    glUseProgram(programID);
}

void Shader::setMat4(const char* name, const glm::mat4& mat) const {
    GLint location = glGetUniformLocation(programID, name);
    glUniformMatrix4fv(location, 1, GL_FALSE, &mat[0][0]);
}

void Shader::setVec3(const char* name, const glm::vec3& vec) const {
    GLint location = glGetUniformLocation(programID, name);
    glUniform3fv(location, 1, &vec[0]);
}

void Shader::setFloat(const char* name, float value) const {
    GLint location = glGetUniformLocation(programID, name);
    glUniform1f(location, value);
}

void Shader::setInt(const char* name, int value) const {
    GLint location = glGetUniformLocation(programID, name);
    glUniform1i(location, value);
}