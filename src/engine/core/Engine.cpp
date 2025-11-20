#include "Engine.h"
#include <iostream>
#include "../src/engine/ecs/Entity.h"
#include "../src/engine/ecs/Component.h"

struct StaticMeshComponent;

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
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    // // Initialize InputManager (After window creation)
    // InputManager::initialize(window);

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

    hdrTexture.loadHDR("../assets/textures/hdri/dark_sky.hdr", 0);

    cubeMaterial.loadAlbedoMap("../assets/textures/pbr/albedo.png");
    cubeMaterial.loadNormalMap("../assets/textures/pbr/normal.png");
    cubeMaterial.loadMetallicMap("../assets/textures/pbr/metallic.png");
    cubeMaterial.loadRoughnessMap("../assets/textures/pbr/roughness.png");
    cubeMaterial.loadAOMap("../assets/textures/pbr/ao.png");

    cubeMaterial.setMetallic(1.0f);      // Non-metallic
    cubeMaterial.setRoughness(0.1f);     // Mid-rough
    cubeMaterial.setAO(1.0f);            // Full ambient occlusion

    envCubemap.fromHDR(hdrTexture, equirectShader);
    glViewport(0, 0, screenWidth, screenHeight); // RESET THE VIEWPORT!!
    skybox.initialize("../assets/shaders/skybox.vert", "../assets/shaders/skybox.frag");

    irradianceMap.generateIrradiance(envCubemap, irradianceShader, 64);
    glViewport(0, 0, screenWidth, screenHeight);

    prefilterMap.generatePrefilter(envCubemap, prefilterShader, 128, 5);
    std::cout << "Prefilter map ID: " << prefilterMap.id << std::endl;
    glViewport(0, 0, screenWidth, screenHeight);

    brdfLUT.generateBRDFLUT(brdfLUTShader, 512);
    std::cout << "BRDF LUT ID: " << brdfLUT.id << std::endl;
    glViewport(0, 0, screenWidth, screenHeight);

    // Setup camera
    camera.updateAspectRatio(screenWidth, screenHeight);
    camera.position = glm::vec3(5.0f, 5.0f, 5.0f);
    camera.target = glm::vec3(0.0f, 2.0f, 0.0f);

    // Setup cube
    // createCube();

    // Optional: Set up state change callback for debugging
    // gameStateManager.setOnStateChange([](GameState oldState, GameState newState) {
    //     std::cout << "[Engine] Game state changed!" << std::endl;
    // });
    //
    // // Setup cube positions
    // cubePositions = {
    //     glm::vec3(0.0f, 0.0f, 0.0f),
    //     glm::vec3(2.5f, 0.0f, 0.0f),
    //     glm::vec3(-2.5f, 0.0f, 0.0f),
    //     glm::vec3(0.0f, 0.0f, 2.5f),
    //     glm::vec3(0.0f, 0.0f, -2.5f),
    //     glm::vec3(2.5f, 0.0f, 2.5f),
    //     glm::vec3(-2.5f, 0.0f, 2.5f),
    //     glm::vec3(2.5f, 0.0f, -2.5f),
    //     glm::vec3(-2.5f, 0.0f, -2.5f),
    //     glm::vec3(0.0f, 2.0f, 0.0f),
    // };

    std::cout << "Engine initialized successfully!" << std::endl;

    // TODO: ABSOLUTELY REMEMBER TO REMOVE THIS
    world.camera = &camera;
   //world.basicShader = &basicShader;
    world.BeginPlay();

    return true;
}

void Engine::run() {
    float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Update input state at the start of each frame
        //InputManager::update();

        processInput();
        update(deltaTime);
        render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void Engine::processInput() {
    //if (InputManager::isKeyPressed(GLFW_KEY_ESCAPE)) {
        // std::cout << "[TEST] ESC pressed via InputManager!" << std::endl;
        //
        // if (gameStateManager.isPlaying() || gameStateManager.isPaused()) {
        //     // In game, ESC toggles pause
        //     gameStateManager.togglePause();
        // } else {
        //     // In menu or game over, ESC quits
        //     glfwSetWindowShouldClose(window, true);
        // }
        //glfwSetWindowShouldClose(window, true);

    //}

    //switch (gameStateManager.getCurrentState()) {
        // case GameState::MENU:
        //     processMenuInput();
        //     break;
        //
        // case GameState::PLAYING:
        //     processGameplayInput();
        //     break;
        //
        // case GameState::PAUSED:
        //     processPauseInput();
        //     break;
        //
        // case GameState::GAME_OVER:
        //     processGameOverInput();
        //     break;
        //
        // case GameState::ROUND_TRANSITION:
        //     // No input during transition
        //     break;
        //
        // case GameState::OPTIONS:
        //     processOptionsInput();
        //     break;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
};

void Engine::update(float deltaTime) {
    world.Update(deltaTime);
}

void Engine::render() {
    // ==== GEOMETRY PASS ====
    gBuffer.bindForWriting();

    // Clear with color
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // TODO: Render stuff here
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
}

void Engine::shutdown() {
    // Temp
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);

    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    cubeMaterial.unbind();
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Engine::createCube() {

}

void Engine::renderEntities() {
    basicShader.use();

    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = camera.getProjectionMatrix();

    basicShader.setMat4("view", view);
    basicShader.setMat4("projection", projection);

    cubeMaterial.bind(basicShader);

    // Draw each entity
    for (auto& entity : world.EntityManager.GetEntities())
    {
        if (entity->HasComponent<StaticMeshComponent>())
        {
            auto& staticMeshComponent = entity->GetComponent<StaticMeshComponent>();

            // Getting the Model.
            glm::mat4 model = staticMeshComponent.GetTransformMatrix();

            basicShader.setMat4("model", model);

            glBindVertexArray(staticMeshComponent.VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }

    // // Bind material (handles textures and uniforms)
    // cubeMaterial.bind(basicShader);
    //
    // glBindVertexArray(cubeVAO);
    //
    // float time = glfwGetTime();
    //
    // for (size_t i = 0; i < cubePositions.size(); ++i) {
    //     glm::mat4 model = glm::mat4(1.0f);
    //     model = glm::translate(model, cubePositions[i]);
    //     float angle = time * glm::radians(20.0f * (i + 1));
    //     model = glm::rotate(model, angle, glm::vec3(0.5f, 1.0f, 0.0f));
    //     model = glm::scale(model, glm::vec3(0.8f));
    //
    //     basicShader.setMat4("model", model);
    //     glDrawArrays(GL_TRIANGLES, 0, 36);
    // }

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
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

void Engine::renderQuad() {
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

//TODO: Move this shit out of the Engine - put in a seperate class or add it in input mamager

// void Engine::processMenuInput() {
//     // SPACE or ENTER to start game
//     if (InputManager::isKeyPressed(GLFW_KEY_SPACE) ||
//         InputManager::isKeyPressed(GLFW_KEY_ENTER)) {
//         std::cout << "[Engine] Starting game from menu!" << std::endl;
//         gameStateManager.setState(GameState::PLAYING);
//     }
//
//     // O for options
//     if (InputManager::isKeyPressed(GLFW_KEY_O)) {
//         std::cout << "[Engine] Opening options menu" << std::endl;
//         gameStateManager.setState(GameState::OPTIONS);
//     }
//
//     // Example: Toggle first point light with L key
//     // static bool lKeyPressed = false;
//     // if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && !lKeyPressed) {
//     //     if (lightManager.getPointLightCount() > 0) {
//     //         for (int i = 0; i < lightManager.getPointLightCount(); i++) {
//     //             auto& light = lightManager.getPointLight(i);
//     //             light.enabled = !light.enabled;
//     //         }
//     //     }
//     //     lKeyPressed = true;
//     // }
//     // if (glfwGetKey(window, GLFW_KEY_L) == GLFW_RELEASE) {
//     //     lKeyPressed = false;
//     // }
// }
//
// void Engine::processGameplayInput() {
//     // P to pause
//     if (InputManager::isKeyPressed(GLFW_KEY_P)) {
//         std::cout << "[Engine] Pausing game" << std::endl;
//         gameStateManager.togglePause();
//     }
//
//     // R to restart (commented out for now to not conflict with test)
//     // if (InputManager::isKeyPressed(GLFW_KEY_R)) {
//     //     std::cout << "[Engine] Restarting game" << std::endl;
//     //     gameStateManager.restartGame();
//     // }
//
//     // Debug keys
//     if (InputManager::isKeyPressed(GLFW_KEY_G)) {
//         std::cout << "[Debug] Game Over triggered" << std::endl;
//         gameStateManager.setState(GameState::GAME_OVER);
//     }
//
//     if (InputManager::isKeyPressed(GLFW_KEY_T)) {
//         std::cout << "[Debug] Round Transition triggered" << std::endl;
//         gameStateManager.setState(GameState::ROUND_TRANSITION);
//     }
// }
//
// void Engine::processPauseInput() {
//     // P to resume (ESC also works via global input)
//     if (InputManager::isKeyPressed(GLFW_KEY_P)) {
//         std::cout << "[Engine] Resuming game" << std::endl;
//         gameStateManager.resumeGame();
//     }
//
//     // M to return to menu
//     if (InputManager::isKeyPressed(GLFW_KEY_M)) {
//         std::cout << "[Engine] Returning to menu from pause" << std::endl;
//         gameStateManager.returnToMenu();
//     }
// }
//
// void Engine::processGameOverInput() {
//     // R to restart (commented out to not conflict with test)
//     // if (InputManager::isKeyPressed(GLFW_KEY_R)) {
//     //     std::cout << "[Engine] Restarting from game over" << std::endl;
//     //     gameStateManager.restartGame();
//     // }
//
//     // M to return to menu
//     if (InputManager::isKeyPressed(GLFW_KEY_M)) {
//         std::cout << "[Engine] Returning to menu from game over" << std::endl;
//         gameStateManager.returnToMenu();
//     }
// }
//
// void Engine::processOptionsInput() {
//     // BACKSPACE to return to menu (ESC also works via global input)
//     if (InputManager::isKeyPressed(GLFW_KEY_BACKSPACE)) {
//         std::cout << "[Engine] Returning to menu from options" << std::endl;
//         gameStateManager.returnToMenu();
//     }
// }
