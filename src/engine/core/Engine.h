#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../renderer/Camera.h"
#include "../renderer/Shader.h"
#include "../renderer/Material.h"
#include "../renderer/GBuffer.h"
#include "../renderer/light/LightManager.h"
#include "../renderer/Cubemap.h"
#include "../renderer/Skybox.h"

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

    Shader basicShader;
    Shader lightingShader;

    Texture hdrTexture;
    Shader equirectShader;
    Cubemap envCubemap;
    Skybox skybox;

    // temp
    GLuint cubeVAO = 0, cubeVBO = 0;

    // Multiple cube positions
    std::vector<glm::vec3> cubePositions;

    // temp too
    void createCube();
    void renderCube();
};
