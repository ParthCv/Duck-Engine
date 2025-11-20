#pragma once
#include <string>
#include <glad/glad.h>

class Cubemap;
class Shader;
class Camera;

class Skybox {
public:
    Skybox();
    ~Skybox();
    
    bool initialize(const std::string& vertPath, const std::string& fragPath);
    void render(const Camera& camera, const Cubemap& cubemap);
private:
    Shader* shader = nullptr;
    GLuint cubeVAO = 0, cubeVBO = 0;
    
    void setupCube();
};