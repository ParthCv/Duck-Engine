#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../renderer/Camera.h"
#include "../renderer/Shader.h"

class Engine {
public:
    bool initialize(int width, int height);
    void run();
    void shutdown();

private:
    GLFWwindow* window = nullptr;
    int screenWidth = 1920;
    int screenHeight = 1080;

    void processInput();
    void update(float deltaTime);
    void render();

    Camera camera;

    // Temp variable
    Shader basicShader;

    GLuint cubeVAO = 0, cubeVBO = 0;

    void createCube();
    void renderCube();

    bool rayIntersectsAABB(const glm::vec3& rayOrigin, const glm::vec3& rayDir,
                           const glm::vec3& aabbMin, const glm::vec3& aabbMax,
                           float& hitDistance);

    glm::vec3 rayStart, rayEnd;

    glm::vec3 cubeAABBMin = glm::vec3(-0.5f, -0.5f, -0.5f);
    glm::vec3 cubeAABBMax = glm::vec3(0.5f, 0.5f, 0.5f);
};