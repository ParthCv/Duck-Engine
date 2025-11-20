#include "Engine.h"
#include <iostream>

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

    // Set viewport
    glViewport(0, 0, screenWidth, screenHeight);
    glEnable(GL_DEPTH_TEST);

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

    hdrTexture.loadHDR("../assets/textures/hdri/dark_sky.hdr", 0);
    std::cout << "HDR texture ID: " << hdrTexture.id << ", size: " << hdrTexture.width << "x" << hdrTexture.height << std::endl;

    cubeMaterial.loadAlbedoMap("../assets/textures/pbr/albedo.png");
    cubeMaterial.loadNormalMap("../assets/textures/pbr/normal.png");
    cubeMaterial.loadMetallicMap("../assets/textures/pbr/metallic.png");
    cubeMaterial.loadRoughnessMap("../assets/textures/pbr/roughness.png");
    cubeMaterial.loadAOMap("../assets/textures/pbr/ao.png");

    cubeMaterial.setMetallic(0.0f);      // Non-metallic
    cubeMaterial.setRoughness(0.5f);     // Mid-rough
    cubeMaterial.setAO(1.0f);            // Full ambient occlusion

    envCubemap.fromHDR(hdrTexture, equirectShader);
    glViewport(0, 0, screenWidth, screenHeight); // RESET THE VIEWPORT!!
    skybox.initialize("../assets/shaders/skybox.vert", "../assets/shaders/skybox.frag");

    DirectionalLight sunLight(
        glm::vec3(-0.5f, -1.0f, -0.3f),
        glm::vec3(1.0f, 0.95f, 0.9f),
        1.0f
    );
    lightManager.addDirectionalLight(sunLight);

    // Add point lights
    PointLight light1(
        glm::vec3(3.0f, 2.0f, 3.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),     // White
        10.0f,
        15.0f
    );
    lightManager.addPointLight(light1);

    PointLight light2(
        glm::vec3(-3.0f, 2.0f, -3.0f),
        glm::vec3(0.2f, 0.5f, 1.0f),     // Blue
        10.0f,
        15.0f
    );
    lightManager.addPointLight(light2);

    PointLight light3(
        glm::vec3(0.0f, 3.0f, 0.0f),
        glm::vec3(1.0f, 0.3f, 0.1f),     // Orange
        8.0f,
        12.0f
    );
    lightManager.addPointLight(light3);

    PointLight light4(
        glm::vec3(-3.0f, 1.0f, 3.0f),
        glm::vec3(0.1f, 1.0f, 0.3f),     // Green
        8.0f,
        12.0f
    );
    lightManager.addPointLight(light4);

    PointLight light5(
        glm::vec3(3.0f, 1.0f, -3.0f),
        glm::vec3(1.0f, 0.1f, 0.8f),     // Purple
        8.0f,
        12.0f
    );
    lightManager.addPointLight(light5);

    // Setup camera
    camera.updateAspectRatio(screenWidth, screenHeight);
    camera.position = glm::vec3(0.0f, 5.0f, 5.0f);
    camera.target = glm::vec3(0.0f, 2.0f, 0.0f);

    // Setup cube
    createCube();

    // Setup cube positions
    cubePositions = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(2.5f, 0.0f, 0.0f),
        glm::vec3(-2.5f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 2.5f),
        glm::vec3(0.0f, 0.0f, -2.5f),
        glm::vec3(2.5f, 0.0f, 2.5f),
        glm::vec3(-2.5f, 0.0f, 2.5f),
        glm::vec3(2.5f, 0.0f, -2.5f),
        glm::vec3(-2.5f, 0.0f, -2.5f),
        glm::vec3(0.0f, 2.0f, 0.0f),
    };

    std::cout << "Engine initialized successfully!" << std::endl;
    std::cout << "Directional lights: " << lightManager.getDirectionalLightCount() << std::endl;
    std::cout << "Point lights: " << lightManager.getPointLightCount() << std::endl;

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
    // Escape Key to exit game for now
    // TODO: remove later maybe
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    // Example: Toggle first point light with L key
    static bool lKeyPressed = false;
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && !lKeyPressed) {
        if (lightManager.getPointLightCount() > 0) {
            for (int i = 0; i < lightManager.getPointLightCount(); i++) {
                auto& light = lightManager.getPointLight(i);
                light.enabled = !light.enabled;
            }
        }
        lKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_RELEASE) {
        lKeyPressed = false;
    }
}

void Engine::update(float deltaTime) {
    float time = glfwGetTime();
    float radius = 10.0f;
    float height = 5.0f;

    camera.position.x = sin(time * 0.5f) * radius;
    camera.position.z = cos(time * 0.5f) * radius;
    camera.position.y = height;

    camera.target = glm::vec3(0.0f, 2.0f, 0.0f);

    if (lightManager.getPointLightCount() > 1) {
        float time = glfwGetTime();
        auto& light = lightManager.getPointLight(1);
        light.position.x = sin(time) * 3.0f;
        light.position.z = cos(time) * 3.0f;
    }
}

void Engine::render() {
    // ==== GEOMETRY PASS ====
    gBuffer.bindForWriting();

    // Clear with color
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // TODO: Render stuff here
    renderCube();

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
    lightManager.uploadToShader(lightingShader);

    lightingShader.setInt("gPosition", 0);
    lightingShader.setInt("gNormal", 1);
    lightingShader.setInt("gAlbedo", 2);
    lightingShader.setInt("gMetallicRoughness", 3);
    gBuffer.bindForReading();

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
    float vertices[] = {
        // Positions          // Normals           // TexCoords
        // Front face (Z+)
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        // Back face (Z-)
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

        // Left face (X-)
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        // Right face (X+)
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        // Bottom face (Y-)
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        // Top face (Y+)
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);

    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // TexCoord (location = 2)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void Engine::renderCube() {
    basicShader.use();

    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = camera.getProjectionMatrix();

    basicShader.setMat4("view", view);
    basicShader.setMat4("projection", projection);

    // Bind material (handles textures and uniforms)
    cubeMaterial.bind(basicShader);

    glBindVertexArray(cubeVAO);

    float time = glfwGetTime();

    for (size_t i = 0; i < cubePositions.size(); ++i) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, cubePositions[i]);
        float angle = time * glm::radians(20.0f * (i + 1));
        model = glm::rotate(model, angle, glm::vec3(0.5f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.8f));

        basicShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
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
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

void Engine::renderQuad() {
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}