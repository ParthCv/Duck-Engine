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
#include "../renderer/ShadowMap.h"
#include "managers/UIManager.h"
#include "managers/UIStateManager.h"
#include "../utils/LoadingScreen.h"


#include "../ecs/system/DebugRenderSystem.h"
#include "../src/engine/ecs/system/RenderingSystem.h"

class Engine {
public:
    World world;

    bool initialize(int width, int height, bool fullscreen);
    void run();
    void shutdown();

    void onResize(int width, int height);

    GBuffer gBuffer;
    ShadowMap shadowMap;
    // Temp
    Material cubeMaterial;
    Material floorMaterial;

    // Temp for GBuffer
    GLuint quadVAO, quadVBO;
    void setupQuad();
    void renderQuad() const;

    UIManager uiManager;
    UIStateManager stateManager;
    DebugRenderSystem debugSystem;
    RenderingSystem renderingSystem;

private:
    GLFWwindow* window = nullptr;
    int screenWidth = 1920;
    int screenHeight = 1080;

    void processInput();
    void update(float deltaTime);
    void render();

    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

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

    LoadingScreen loadingScreen;

    Shader physicsDebugShader;
    bool bPhysicsDebug = false;

    GLuint floorVAO, floorVBO;

    void createFloor();
    void renderFloor();

    void renderEntities();
    void handleStateChange(GameState oldState, GameState newState);

};
