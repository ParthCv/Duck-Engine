#include "Engine.h"
#include <iostream>
#include "../src/engine/ecs/Entity.h"
#include "../src/engine/ecs/Component.h"
#include "../system/DebugRenderSystem.h"

#include "managers/AudioManager.h"
#include "managers/GameStateManager.h"
#include "managers/InputManager.h"

struct StaticMeshComponent;

// Global instances since we cannot modify Engine.h easily

bool Engine::initialize(int width, int height, bool fullscreen) {
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

    GLFWmonitor* monitor = nullptr;
    if (fullscreen) {
        monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        screenWidth = mode->width;
        screenHeight = mode->height;

        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
    }

    window = glfwCreateWindow(screenWidth, screenHeight, "Game", monitor, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // Initialize GLAD
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    InputManager::initialize(window);
    uiManager.initialize(screenWidth, screenHeight);

    // Setup loading screen
    glViewport(0, 0, screenWidth, screenHeight);
    loadingScreen.initialize(screenWidth, screenHeight);
    float loadTime = 0.0f;
    auto updateLoadingScreen = [&]() {
        loadTime += 0.1f;
        glViewport(0, 0, screenWidth, screenHeight);  // Reset viewport
        glDisable(GL_DEPTH_TEST);
        loadingScreen.render(glfwGetTime());
        glfwSwapBuffers(window);
        glfwPollEvents();
    };

    updateLoadingScreen();

    // Setup state change callback to update UI
    stateManager.setOnStateChange([this](GameState oldState, GameState newState) {
        handleStateChange(oldState, newState);
    });

    // Setup initial UI (menu screen)
    uiManager.setupMenuUI(&stateManager);

    // Set viewport
    glViewport(0, 0, screenWidth, screenHeight);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    updateLoadingScreen();
    gBuffer.initialize(screenWidth, screenHeight);
    setupQuad();

    updateLoadingScreen();
    shadowMap.initialize(world.lightManager);

    // Load shaders
    updateLoadingScreen();
    if (!equirectShader.loadFromFiles("../assets/shaders/equirect_to_cubemap.vert", "../assets/shaders/equirect_to_cubemap.frag")) {
        std::cerr << "Failed to load basic shaders" << std::endl;
        return false;
    }

    updateLoadingScreen();
    if (!basicShader.loadFromFiles("../assets/shaders/geometry.vert", "../assets/shaders/geometry.frag")) {
        std::cerr << "Failed to load basic shaders" << std::endl;
        return false;
    }

    updateLoadingScreen();
    if (!lightingShader.loadFromFiles("../assets/shaders/light.vert", "../assets/shaders/light.frag")) {
        std::cerr << "Failed to load light shaders" << std::endl;
        return false;
    }

    updateLoadingScreen();
    if (!irradianceShader.loadFromFiles("../assets/shaders/irradiance_cubemap.vert", "../assets/shaders/irradiance_cubemap.frag")) {
        std::cerr << "Failed to load irradiance shader" << std::endl;
        return false;
    }

    updateLoadingScreen();
    if (!prefilterShader.loadFromFiles("../assets/shaders/prefilter.vert", "../assets/shaders/prefilter.frag")) {
        std::cerr << "Failed to load prefilter shader" << std::endl;
        return false;
    }

    updateLoadingScreen();
    if (!brdfLUTShader.loadFromFiles("../assets/shaders/brdf_lut.vert", "../assets/shaders/brdf_lut.frag")) {
        std::cerr << "Failed to load BRDF shader" << std::endl;
        return false;
    }

    updateLoadingScreen();
    if (!physicsDebugShader.loadFromFiles("../assets/shaders/physics_debug.vert", "../assets/shaders/physics_debug.frag")) {
        std::cerr << "Failed to load debug shader" << std::endl;
        return false;
    }

    updateLoadingScreen();
    hdrTexture.loadHDR("../assets/textures/hdri/dark_sky.hdr", 0);

    updateLoadingScreen();
    cubeMaterial.loadAlbedoMap("../assets/textures/pbr/albedo.png");
    updateLoadingScreen();
    cubeMaterial.loadNormalMap("../assets/textures/pbr/normal.png");
    updateLoadingScreen();
    cubeMaterial.loadMetallicMap("../assets/textures/pbr/metallic.png");
    updateLoadingScreen();
    cubeMaterial.loadRoughnessMap("../assets/textures/pbr/roughness.png");
    updateLoadingScreen();
    cubeMaterial.loadAOMap("../assets/textures/pbr/ao.png");

    floorMaterial.loadAlbedoMap("../assets/textures/pbr_ground/albedo.png");
    // floorMaterial.loadAOMap("../assets/textures/pbr_ground/ao.png");
    floorMaterial.loadNormalMap("../assets/textures/pbr_ground/normals.png");
    floorMaterial.loadRoughnessMap("../assets/textures/pbr_ground/roughness.png");

    updateLoadingScreen();
    cubeMaterial.setMetallic(1.0f);      // Non-metallic
    cubeMaterial.setRoughness(0.1f);     // Mid-rough
    cubeMaterial.setAO(1.0f);            // Full ambient occlusion

    updateLoadingScreen();
    envCubemap.fromHDR(hdrTexture, equirectShader);
    glViewport(0, 0, screenWidth, screenHeight); // RESET THE VIEWPORT!!
    skybox.initialize("../assets/shaders/skybox.vert", "../assets/shaders/skybox.frag");

    updateLoadingScreen();
    irradianceMap.generateIrradiance(envCubemap, irradianceShader, 64);
    glViewport(0, 0, screenWidth, screenHeight);

    updateLoadingScreen();
    prefilterMap.generatePrefilter(envCubemap, prefilterShader, 128, 5);
    std::cout << "Prefilter map ID: " << prefilterMap.id << std::endl;
    glViewport(0, 0, screenWidth, screenHeight);

    updateLoadingScreen();
    brdfLUT.generateBRDFLUT(brdfLUTShader, 512);
    std::cout << "BRDF LUT ID: " << brdfLUT.id << std::endl;
    glViewport(0, 0, screenWidth, screenHeight);

    updateLoadingScreen();
    // Initialize Debug Renderer
    debugSystem.init();

    createFloor();

    updateLoadingScreen();

    // Setup camera
    camera.updateAspectRatio(screenWidth, screenHeight);
    camera.position = glm::vec3(5.0f, 5.0f, 5.0f);

    std::cout << "Engine initialized successfully!" << std::endl;

    updateLoadingScreen();

    world.camera = &camera;
    world.beginPlay();

    updateLoadingScreen();

    InputManager::initialize(window);
    AudioManager::Get().Init();
    stateManager.setWorldContext(&world);

    return true;
}

void Engine::run() {
    float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput();
        update(deltaTime);
        render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void Engine::processInput() {
    // ESC key closes window
    if (InputManager::isKeyPressed(GLFW_KEY_ESCAPE)) {
        if (stateManager.getCurrentState() == GameState::PLAYING) {
            stateManager.togglePause();
        } else {
            glfwSetWindowShouldClose(window, true);
        }
    }

    // P key toggles pause
    if (InputManager::isKeyPressed(GLFW_KEY_P)) {
        stateManager.togglePause();
    }

    // Debug physics toggle
    static bool cKeyPressed = false;
    if (InputManager::isKeyDown(GLFW_KEY_C) && !cKeyPressed) {
        bPhysicsDebug = !bPhysicsDebug;
        cKeyPressed = true;
    }
    if (!InputManager::isKeyDown(GLFW_KEY_C)) {
        cKeyPressed = false;
    }
}

void Engine::update(float deltaTime) {
    // Update input first (Always needed for UI navigation)
    InputManager::update();

    // Update game state (Handles camera inputs when playing, UI logic, Etc)
    stateManager.update(deltaTime);

    // Update UI (Always needed)
    uiManager.update(deltaTime);

    // Update world (Physics, Entities, Lights, Spawning)
    // ONLY update the world simulation if we are in the PLAYING state.
    if (stateManager.getCurrentState() == GameState::PLAYING) {
        world.update(deltaTime);
    }

    // static float garbageTimer = 0.0f;
    // garbageTimer += deltaTime;
    // if (garbageTimer >= 5.0f) {
    //     ResourceManager::Get().CollectGarbage();
    //     garbageTimer = 0.0f;
    // }
}

void Engine::render() {
    //  ==== SHADOW PASS ====
    // TODO: REMOVE - If final game does not have dynamic directional light
    shadowMap.updateLightSpaceTransform(world.lightManager);  // Synchronize with LightManager

    shadowMap.render(world);

    // ==== GEOMETRY PASS ====
    gBuffer.bindForWriting();

    // Clear with color
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderFloor();
    renderEntities();

    // ==== LIGHTING PASS ====
    // Unbind GBuffer framebuffer and switch back to screen
    gBuffer.unbind();
    glViewport(0, 0, screenWidth, screenHeight);

    // DISPLAY PASS
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    lightingShader.use();
    lightingShader.setVec3("viewPos", camera.position);

    // Get all lights and pass to shader
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

    glActiveTexture(GL_TEXTURE0 + 7);
    glBindTexture(GL_TEXTURE_2D, shadowMap.getDepthMap());
    lightingShader.setInt("shadowMap", 7);
    lightingShader.setMat4("lightSpaceMatrix", shadowMap.getLightSpaceMatrix());

    // Render final quad
    renderQuad();

    glEnable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer.getFramebuffer());  // GBuffer's framebuffer ID
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);  // Default framebuffer
    glBlitFramebuffer(
        0, 0, screenWidth, screenHeight,
        0, 0, screenWidth, screenHeight,
        GL_DEPTH_BUFFER_BIT, GL_NEAREST
    );
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glEnable(GL_DEPTH_TEST);
    skybox.render(camera, envCubemap);

    // ==== DEBUG RENDER PASS ====
    if (bPhysicsDebug) {
        physicsDebugShader.use();
        physicsDebugShader.setMat4("view", camera.getViewMatrix());
        physicsDebugShader.setMat4("projection", camera.getProjectionMatrix());
        debugSystem.render(world.EntityManager, physicsDebugShader);
    }

    // Handle UI last
    uiManager.render();

}

void Engine::framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
    engine->onResize(width, height);
}

void Engine::shutdown() {
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    cubeMaterial.unbind();
    uiManager.shutdown();
    debugSystem.cleanup();

    glDeleteVertexArrays(1, &floorVAO);
    glDeleteBuffers(1, &floorVBO);

    glfwDestroyWindow(window);
    glfwTerminate();
}

void Engine::onResize(int width, int height) {
    screenWidth = width;
    screenHeight = height;

    glViewport(0, 0, width, height);

    // Resize GBuffer
    gBuffer.resize(width, height);

    // Update camera aspect ratio
    camera.updateAspectRatio(width, height);

    std::cout << "Window resized to: " << width << "x" << height << std::endl;
}

void Engine::renderEntities() {
    basicShader.use();

    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = camera.getProjectionMatrix();

    basicShader.setMat4("view", view);
    basicShader.setMat4("projection", projection);

    // TODO: store a list of renderable entities to iterate instead
    // Draw each entity
    std::unordered_map<Material*, std::vector<StaticMeshComponent*>> materialBatches;

    for (auto& entity : world.EntityManager.GetEntities())
    {
        if (entity == nullptr) continue;
        if (entity->hasComponent<StaticMeshComponent>())
        {
            auto& staticMeshComponent = entity->getComponent<StaticMeshComponent>();
            Material* mat = staticMeshComponent.material ? staticMeshComponent.material.get() : &cubeMaterial;
            materialBatches[mat].push_back(&staticMeshComponent);
        }
    }

    // Render batched by material
    for (auto& [material, components] : materialBatches)
    {
        material->bind(basicShader);

        for (auto* comp : components)
        {
            glm::mat4 model = comp->getTransformMatrix();
            basicShader.setMat4("model", model);

            comp->Mesh->bind();
            comp->Mesh->draw();
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

void Engine::handleStateChange(GameState oldState, GameState newState) {
    std::cout << "[Engine] State changed from " << stateManager.getStateString()
              << " to new state" << std::endl;

    if (newState == GameState::PLAYING) {
        // Lock and hide the cursor for active 3D gameplay
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        std::cout << "[Engine] Cursor DISABLED for PLAYING" << std::endl;
    } else {
        // Show and unlock the cursor for UI interaction (Menu, Paused, Options, Game Over)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        std::cout << "[Engine] Cursor NORMAL for UI" << std::endl;
    }

    // Clear previous UI
    switch (oldState) {
        case GameState::MENU:
            uiManager.clearMenuUI();
            break;
        case GameState::PLAYING:
            uiManager.clearPlayingUI();
            break;
        case GameState::PAUSED:
            uiManager.clearPausedUI();
            break;
        case GameState::GAME_OVER:
            uiManager.clearGameOverUI();
            break;
        case GameState::OPTIONS:
            uiManager.clearAll();
            break;
    }

    // Setup new UI
    switch (newState) {
        case GameState::MENU:
            uiManager.setupMenuUI(&stateManager);
            break;
        case GameState::PLAYING:
            uiManager.setupPlayingUI();
            break;
        case GameState::PAUSED:
            // Keep playing UI visible, add pause overlay on top
            uiManager.setupPausedUI(&stateManager);
            break;
        case GameState::GAME_OVER:
            uiManager.setupGameOverUI(&stateManager);
            break;
        case GameState::OPTIONS:
            uiManager.setupOptionsUI(&stateManager);
            break;
    }
}


void Engine::createFloor() {
    float floorSize = 80.0f;  // Large but not truly infinite
    float floorVertices[] = {
        // Positions          // Normals           // TexCoords
        -floorSize, 1.5f, -floorSize,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
         floorSize, 1.5f, -floorSize,  0.0f, 1.0f, 0.0f,  10.0f, 0.0f,
         floorSize, 1.5f,  floorSize,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f,
         floorSize, 1.5f,  floorSize,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f,
        -floorSize, 1.5f,  floorSize,  0.0f, 1.0f, 0.0f,  0.0f, 10.0f,
        -floorSize, 1.5f, -floorSize,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f
    };

    glGenVertexArrays(1, &floorVAO);
    glGenBuffers(1, &floorVBO);

    glBindVertexArray(floorVAO);
    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // TexCoord
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void Engine::renderFloor() {
    basicShader.use();

    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = camera.getProjectionMatrix();
    auto model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));  // Lower the floor

    basicShader.setMat4("model", model);
    basicShader.setMat4("view", view);
    basicShader.setMat4("projection", projection);

    // Use same material or create a floor material
    floorMaterial.bind(basicShader);

    glBindVertexArray(floorVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
