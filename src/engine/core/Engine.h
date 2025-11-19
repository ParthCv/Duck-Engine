#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../renderer/Camera.h"
#include "../renderer/Shader.h"
#include "../renderer/Material.h"
#include "../renderer/GBuffer.h"
#include "../renderer/light/LightManager.h"

class Engine {
public:
    bool initialize(int width, int height);
    void run();
    void shutdown();

    GBuffer gBuffer;
    LightManager lightManager;
    // Temp
    Material cubeMaterial;
    Shader displayShader;
    // Temp for GBuffer
    GLuint quadVAO, quadVBO;
    void setupQuad();
    void renderQuad();

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
    Shader lightingShader;

    GLuint cubeVAO = 0, cubeVBO = 0;

    void createCube();
    void renderCube();
};
