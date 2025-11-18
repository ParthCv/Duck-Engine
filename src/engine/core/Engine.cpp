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

    // Initialize InputManager (After window creation)
    InputManager::initialize(window);

    // Set viewport
    glViewport(0, 0, screenWidth, screenHeight);
    glEnable(GL_DEPTH_TEST);

    // Temp code for now

    // Load shader
    if (!basicShader.loadFromFiles("../assets/shaders/basic.vert", "../assets/shaders/basic.frag")) {
        std::cerr << "Failed to load shaders" << std::endl;
        return false;
    }

    // Setup camera
    camera.updateAspectRatio(screenWidth, screenHeight);
    camera.position = glm::vec3(0.0f, 0.0f, 3.0f);

    // Setup cube
    // createCube();

    // Optional: Set up state change callback for debugging
    gameStateManager.setOnStateChange([](GameState oldState, GameState newState) {
        std::cout << "[Engine] Game state changed!" << std::endl;
    });

    std::cout << "Engine initialized successfully!" << std::endl;

    // TODO: ABSOLUTELY REMEMBER TO REMOVE THIS
    World.camera = &camera;
    World.basicShader = &basicShader;
    World.BeginPlay();

    return true;
}

void Engine::run() {
    float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Update input state at the start of each frame
        InputManager::update();

        processInput();
        update(deltaTime);
        render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void Engine::processInput() {
    ////////////////////////////////////////////////////////////////////////
    // GLOBAL INPUT (works in all states)
    ////////////////////////////////////////////////////////////////////////

    // ESC - Context-dependent behavior
    if (InputManager::isKeyPressed(GLFW_KEY_ESCAPE)) {
        std::cout << "[TEST] ESC pressed via InputManager!" << std::endl;

        if (gameStateManager.isPlaying() || gameStateManager.isPaused()) {
            // In game, ESC toggles pause
            gameStateManager.togglePause();
        } else {
            // In menu or game over, ESC quits
            glfwSetWindowShouldClose(window, true);
        }
    }

    ////////////////////////////////////////////////////////////////////////
    // STATE-SPECIFIC INPUT
    ////////////////////////////////////////////////////////////////////////

    switch (gameStateManager.getCurrentState()) {
        case GameState::MENU:
            processMenuInput();
            break;

        case GameState::PLAYING:
            processGameplayInput();
            break;

        case GameState::PAUSED:
            processPauseInput();
            break;

        case GameState::GAME_OVER:
            processGameOverInput();
            break;

        case GameState::ROUND_TRANSITION:
            // No input during transition
            break;

        case GameState::OPTIONS:
            processOptionsInput();
            break;
    }

    ////////////////////////////////////////////////////////////////////////
    // NORMAN'S TEST CODE (keeping for now)
    ////////////////////////////////////////////////////////////////////////

    // Test 2: SPACE single press
    if (InputManager::isKeyPressed(GLFW_KEY_SPACE)) {
        std::cout << "[TEST] SPACE just pressed!" << std::endl;
    }

    // Test 3: W held down
    if (InputManager::isKeyDown(GLFW_KEY_W)) {
        std::cout << "[TEST] W is held down" << std::endl;
    }

    // Test 4: Key release
    if (InputManager::isKeyReleased(GLFW_KEY_R)) {
        std::cout << "[TEST] R was released!" << std::endl;
    }

    // Test 5: Mouse click
    if (InputManager::isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
        glm::vec2 mousePos = InputManager::getMousePosition();
        std::cout << "[TEST] MOUSE CLICKED at (" << mousePos.x << ", " << mousePos.y << ")" << std::endl;
    }

    // Test 6: Mouse position (Very spammy, commmented out)
    // static int frameCount = 0;
    // frameCount++;
    // if (frameCount % 60 == 0) { // Every second
    //     glm::vec2 mousePos = InputManager::getMousePosition();
    //     std::cout << "[TEST] Mouse position: (" << mousePos.x << ", " << mousePos.y << ")" << std::endl;
    // }

    // // Test 7: Mouse delta (movement) (Spammy, commented out)
    // glm::vec2 mouseDelta = InputManager::getMouseDelta();
    // if (glm::length(mouseDelta) > 5.0f) { // If mouse moved significantly
    //     std::cout << "[TEST] Mouse moved: (" << mouseDelta.x << ", " << mouseDelta.y << ")" << std::endl;
    // }

}

// ============================================================================
// STATE-SPECIFIC INPUT HANDLERS
// ============================================================================

void Engine::processMenuInput() {
    // SPACE or ENTER to start game
    if (InputManager::isKeyPressed(GLFW_KEY_SPACE) ||
        InputManager::isKeyPressed(GLFW_KEY_ENTER)) {
        std::cout << "[Engine] Starting game from menu!" << std::endl;
        gameStateManager.setState(GameState::PLAYING);
    }

    // O for options
    if (InputManager::isKeyPressed(GLFW_KEY_O)) {
        std::cout << "[Engine] Opening options menu" << std::endl;
        gameStateManager.setState(GameState::OPTIONS);
    }
}

void Engine::processGameplayInput() {
    // P to pause
    if (InputManager::isKeyPressed(GLFW_KEY_P)) {
        std::cout << "[Engine] Pausing game" << std::endl;
        gameStateManager.togglePause();
    }

    // R to restart (commented out for now to not conflict with test)
    // if (InputManager::isKeyPressed(GLFW_KEY_R)) {
    //     std::cout << "[Engine] Restarting game" << std::endl;
    //     gameStateManager.restartGame();
    // }

    // Debug keys
    if (InputManager::isKeyPressed(GLFW_KEY_G)) {
        std::cout << "[Debug] Game Over triggered" << std::endl;
        gameStateManager.setState(GameState::GAME_OVER);
    }

    if (InputManager::isKeyPressed(GLFW_KEY_T)) {
        std::cout << "[Debug] Round Transition triggered" << std::endl;
        gameStateManager.setState(GameState::ROUND_TRANSITION);
    }
}

void Engine::processPauseInput() {
    // P to resume (ESC also works via global input)
    if (InputManager::isKeyPressed(GLFW_KEY_P)) {
        std::cout << "[Engine] Resuming game" << std::endl;
        gameStateManager.resumeGame();
    }

    // M to return to menu
    if (InputManager::isKeyPressed(GLFW_KEY_M)) {
        std::cout << "[Engine] Returning to menu from pause" << std::endl;
        gameStateManager.returnToMenu();
    }
}

void Engine::processGameOverInput() {
    // R to restart (commented out to not conflict with test)
    // if (InputManager::isKeyPressed(GLFW_KEY_R)) {
    //     std::cout << "[Engine] Restarting from game over" << std::endl;
    //     gameStateManager.restartGame();
    // }

    // M to return to menu
    if (InputManager::isKeyPressed(GLFW_KEY_M)) {
        std::cout << "[Engine] Returning to menu from game over" << std::endl;
        gameStateManager.returnToMenu();
    }
}

void Engine::processOptionsInput() {
    // BACKSPACE to return to menu (ESC also works via global input)
    if (InputManager::isKeyPressed(GLFW_KEY_BACKSPACE)) {
        std::cout << "[Engine] Returning to menu from options" << std::endl;
        gameStateManager.returnToMenu();
    }
}

// ============================================================================
// UPDATE AND RENDER
// ============================================================================

void Engine::update(float deltaTime) {
    // Update the game state manager
    gameStateManager.update(deltaTime);

    // Only update world when actually playing
    if (gameStateManager.isPlaying()) {

        World.Update(deltaTime);
        // Update game logic (ducks, score, etc.) - will add later

    }
}

void Engine::render() {

    // Clear ONCE at the start
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render game scene through World
    if (gameStateManager.isPlaying() || gameStateManager.isPaused()) {
        World.Render();  // World renders all entities
    }

    // Render UI/HUD on top
    gameStateManager.render();  // This should only render UI overlays
}

void Engine::shutdown() {
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);

    glfwDestroyWindow(window);
    glfwTerminate();
}

void Engine::createCube() {
    // Cube vertices with colors
    float vertices[] = {
        // Positions          // Colors
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f, // Front face (red)
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f, // Back face (green)
         0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f, // Left face (blue)
        -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f, // Right face (yellow)
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f, // Bottom face (magenta)
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f, // Top face (cyan)
         0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f
    };

    // Generate VAO and VBO
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);

    // Bind and upload data
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}
