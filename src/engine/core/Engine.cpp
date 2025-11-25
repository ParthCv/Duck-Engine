#include "Engine.h"
#include <iostream>
#include "../src/engine/ecs/Entity.h"
#include "../src/engine/ecs/Component.h"
#include "../system/DebugRenderSystem.h"

// INCLUDE NEW MANAGERS
#include "../game/GameStateManager.h"
#include "../input/InputManager.h"

struct StaticMeshComponent;

// Global instances since we cannot modify Engine.h easily
DebugRenderSystem debugSystem;
GameStateManager gameStateManager;

bool Engine::initialize(int width, int height) {
    screenWidth = width;
    screenHeight = height;

    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    // OpenGL 3.3 Core check
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    window = glfwCreateWindow(screenWidth, screenHeight, "Duck Hunt 3D", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);

    // Initialize GLAD
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    // Set viewport
    glViewport(0, 0, screenWidth, screenHeight);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    gBuffer.initialize(screenWidth, screenHeight);
    setupQuad();

    // Load shaders
    if (!equirectShader.loadFromFiles("../assets/shaders/equirect_to_cubemap.vert", "../assets/shaders/equirect_to_cubemap.frag")) {
        std::cerr << "Failed to load basic shaders" << std::endl;
        return false;
    }
    if (!basicShader.loadFromFiles("../assets/shaders/geometry.vert", "../assets/shaders/geometry.frag")) {
        std::cerr << "Failed to load basic shaders" << std::endl;
        return false;
    }
    if (!lightingShader.loadFromFiles("../assets/shaders/light.vert", "../assets/shaders/light.frag")) {
        std::cerr << "Failed to load light shaders" << std::endl;
        return false;
    }
    if (!irradianceShader.loadFromFiles("../assets/shaders/irradiance_cubemap.vert", "../assets/shaders/irradiance_cubemap.frag")) {
        std::cerr << "Failed to load irradiance shader" << std::endl;
        return false;
    }
    if (!prefilterShader.loadFromFiles("../assets/shaders/prefilter.vert", "../assets/shaders/prefilter.frag")) {
        std::cerr << "Failed to load prefilter shader" << std::endl;
        return false;
    }
    if (!brdfLUTShader.loadFromFiles("../assets/shaders/brdf_lut.vert", "../assets/shaders/brdf_lut.frag")) {
        std::cerr << "Failed to load BRDF shader" << std::endl;
        return false;
    }
    if (!physicsDebugShader.loadFromFiles("../assets/shaders/physics_debug.vert", "../assets/shaders/physics_debug.frag")) {
        std::cerr << "Failed to load debug shader" << std::endl;
        return false;
    }

    hdrTexture.loadHDR("../assets/textures/hdri/dark_sky.hdr", 0);

    cubeMaterial.loadAlbedoMap("../assets/textures/pbr/albedo.png");
    cubeMaterial.loadNormalMap("../assets/textures/pbr/normal.png");
    cubeMaterial.loadMetallicMap("../assets/textures/pbr/metallic.png");
    cubeMaterial.loadRoughnessMap("../assets/textures/pbr/roughness.png");
    cubeMaterial.loadAOMap("../assets/textures/pbr/ao.png");

    cubeMaterial.setMetallic(1.0f);
    cubeMaterial.setRoughness(0.1f);
    cubeMaterial.setAO(1.0f);

    envCubemap.fromHDR(hdrTexture, equirectShader);
    glViewport(0, 0, screenWidth, screenHeight);
    skybox.initialize("../assets/shaders/skybox.vert", "../assets/shaders/skybox.frag");

    irradianceMap.generateIrradiance(envCubemap, irradianceShader, 64);
    glViewport(0, 0, screenWidth, screenHeight);

    prefilterMap.generatePrefilter(envCubemap, prefilterShader, 128, 5);
    glViewport(0, 0, screenWidth, screenHeight);

    brdfLUT.generateBRDFLUT(brdfLUTShader, 512);
    glViewport(0, 0, screenWidth, screenHeight);

    // Initialize Debug Renderer
    debugSystem.init();

    // Setup camera
    camera.updateAspectRatio(screenWidth, screenHeight);
    camera.position = glm::vec3(5.0f, 5.0f, 5.0f);
    camera.target = glm::vec3(0.0f, 2.0f, 0.0f);

    std::cout << "Engine initialized successfully!" << std::endl;

    world.camera = &camera;
    world.beginPlay();

    // === INITIALIZE NEW MANAGERS ===
    InputManager::initialize(window);
    gameStateManager.setWorldContext(&world);

    return true;
}

void Engine::run() {
    float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // === UPDATE INPUT MANAGER ===
        InputManager::update();

        processInput();
        update(deltaTime);
        render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void Engine::processInput() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    static bool cKeyPressed = false;
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && !cKeyPressed) {
        bPhysicsDebug = !bPhysicsDebug;
        cKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE) {
        cKeyPressed = false;
    }
};

void Engine::update(float deltaTime) {
    // Instead of calling world.update() directly, we update via GameStateManager
    gameStateManager.update(deltaTime);
}

void Engine::render() {
    // ==== GEOMETRY PASS ====
    gBuffer.bindForWriting();

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderEntities();

    // ==== LIGHTING PASS ====
    gBuffer.unbind();
    glViewport(0, 0, screenWidth, screenHeight);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    lightingShader.use();
    lightingShader.setVec3("viewPos", camera.position);
    world.lightManager.uploadToShader(lightingShader);

    lightingShader.setInt("gPosition", 0);
    lightingShader.setInt("gNormal", 1);
    lightingShader.setInt("gAlbedo", 2);
    lightingShader.setInt("gMetallicRoughness", 3);
    gBuffer.bindForReading();

    lightingShader.setInt("irradianceMap", 4);
    irradianceMap.bind(4);

    lightingShader.setInt("prefilterMap", 5);
    prefilterMap.bind(5);

    lightingShader.setInt("brdfLUT", 6);
    brdfLUT.textureUnit = 6;
    brdfLUT.bind();

    renderQuad();

    glEnable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer.getFramebuffer());
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(
        0, 0, screenWidth, screenHeight,
        0, 0, screenWidth, screenHeight,
        GL_DEPTH_BUFFER_BIT, GL_NEAREST
    );
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glEnable(GL_DEPTH_TEST);
    skybox.render(camera, envCubemap);

    // ==== GAME STATE UI RENDER ====
    // If you have UI, it renders here
    gameStateManager.render();

    // ==== DEBUG RENDER PASS ====
    if (bPhysicsDebug) {
        physicsDebugShader.use();
        physicsDebugShader.setMat4("view", camera.getViewMatrix());
        physicsDebugShader.setMat4("projection", camera.getProjectionMatrix());
        debugSystem.render(world.EntityManager, physicsDebugShader);
    }
}

void Engine::shutdown() {
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    cubeMaterial.unbind();
    debugSystem.cleanup();
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Engine::renderEntities() {
    basicShader.use();

    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = camera.getProjectionMatrix();

    basicShader.setMat4("view", view);
    basicShader.setMat4("projection", projection);

    cubeMaterial.bind(basicShader);

    for (auto& entity : world.EntityManager.GetEntities())
    {
        if (entity == nullptr) continue;
        if (entity->hasComponent<StaticMeshComponent>())
        {
            auto& staticMeshComponent = entity->getComponent<StaticMeshComponent>();
            glm::mat4 model = staticMeshComponent.getTransformMatrix();
            basicShader.setMat4("model", model);
            staticMeshComponent.Mesh->bind();
            staticMeshComponent.Mesh->draw();
        }
    }
    glBindVertexArray(0);
}

void Engine::setupQuad() {
    float quadVertices[] = {
        // pos        // tex
        -1.0f,  1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
    };
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), static_cast<void *>(nullptr));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void *>(2 * sizeof(float)));
}

void Engine::renderQuad() const {
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}