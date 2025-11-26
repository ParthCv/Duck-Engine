#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../ecs/World.h"
#include "../renderer/Camera.h"
#include "../renderer/Shader.h"
#include "../renderer/Material.h"
#include "../renderer/GBuffer.h"
#include "../renderer/Cubemap.h"
#include "../renderer/Skybox.h"
#include "managers/UIManager.h"
#include "managers/GameStateManager.h"
#include "../renderer/ShadowMap.h"


class Engine {
public:
    World world;

    bool initialize(int width, int height);
    void run();
    void shutdown();

    GBuffer gBuffer;
    ShadowMap shadowMap;
    // Temp
    Material cubeMaterial;

    // Temp for GBuffer
    GLuint quadVAO, quadVBO;
    void setupQuad();
    void renderQuad() const;

    UIManager uiManager;
    GameStateManager stateManager;

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

    Cubemap irradianceMap;
    Shader irradianceShader;

    Shader prefilterShader;
    Cubemap prefilterMap;

    Shader brdfLUTShader;
    Texture brdfLUT;

    Texture shadowMapTexture;

    Shader physicsDebugShader;
    bool bPhysicsDebug = false;

    GLuint floorVAO, floorVBO;

    void createFloor();
    void renderFloor();

    void renderEntities();
    void handleStateChange(GameState oldState, GameState newState);

};
