#include <glad/glad.h> // MUST be included before GLFW
#include "GameStateManager.h"
#include <iostream>

#include "../ecs/World.h"
#include "../ecs/Component.h"
#include "../input/InputUtils.h"
#include "../system/CollisionSystem.h"
#include "DuckEntity.h"

GameStateManager::GameStateManager()
    : currentState(GameState::PLAYING)
    , previousState(GameState::MENU)
    , stateTimer(0.0f)
    , onStateChangeCallback(nullptr)
    , worldContext(nullptr)
{
    std::cout << "[GameStateManager] Initialized - Starting in PLAYING state" << std::endl;
}

void GameStateManager::setState(GameState newState) {
    if (newState == currentState) {
        return;
    }

    std::cout << "[GameStateManager] State transition: "
              << getStateString() << " -> ";

    previousState = currentState;
    currentState = newState;
    stateTimer = 0.0f;

    std::cout << getStateString() << std::endl;

    notifyStateChange(previousState, currentState);
}

void GameStateManager::update(float deltaTime) {
    stateTimer += deltaTime;

    switch (currentState) {
        case GameState::MENU:
            updateMenu(deltaTime);
            break;
        case GameState::PLAYING:
            updatePlaying(deltaTime);
            break;
        case GameState::PAUSED:
            updatePaused(deltaTime);
            break;
        case GameState::ROUND_TRANSITION:
            updateRoundTransition(deltaTime);
            break;
        case GameState::GAME_OVER:
            updateGameOver(deltaTime);
            break;
        case GameState::OPTIONS:
            updateOptions(deltaTime);
            break;
    }
}

void GameStateManager::render() {
    switch (currentState) {
        case GameState::MENU:
            renderMenu();
            break;
        case GameState::PLAYING:
            renderPlaying();
            break;
        case GameState::PAUSED:
            renderPaused();
            break;
        case GameState::ROUND_TRANSITION:
            renderRoundTransition();
            break;
        case GameState::GAME_OVER:
            renderGameOver();
            break;
        case GameState::OPTIONS:
            renderOptions();
            break;
    }
}

void GameStateManager::togglePause() {
    if (currentState == GameState::PLAYING) {
        setState(GameState::PAUSED);
    } else if (currentState == GameState::PAUSED) {
        setState(GameState::PLAYING);
    }
}

void GameStateManager::resumeGame() {
    if (currentState == GameState::PAUSED) {
        setState(GameState::PLAYING);
    }
}

void GameStateManager::restartGame() {
    std::cout << "[GameStateManager] restarting game..." << std::endl;
    setState(GameState::PLAYING);
}

void GameStateManager::returnToMenu() {
    std::cout << "[GameStateManager] returning to menu..." << std::endl;
    setState(GameState::MENU);
}

std::string GameStateManager::getStateString() const {
    switch (currentState) {
        case GameState::MENU: return "MENU";
        case GameState::PLAYING: return "PLAYING";
        case GameState::PAUSED: return "PAUSED";
        case GameState::ROUND_TRANSITION: return "ROUND_TRANSITION";
        case GameState::GAME_OVER: return "GAME_OVER";
        case GameState::OPTIONS: return "OPTIONS";
        default: return "UNKNOWN";
    }
}

void GameStateManager::notifyStateChange(GameState oldState, GameState newState) {
    if (onStateChangeCallback) {
        onStateChangeCallback(oldState, newState);
    }
}

// ============================================================================
// STATE-SPECIFIC UPDATE METHODS
// ============================================================================

void GameStateManager::updateMenu(float deltaTime) {
    // TODO: Menu logic here
}

void GameStateManager::updatePlaying(float deltaTime) {
    // Guard against null world or camera
    if (!worldContext || !worldContext->camera) return;

    // --- CALCULATE CAMERA VECTORS ---
    glm::mat4 view = worldContext->camera->getViewMatrix();
    // Row 0 = Right, Row 1 = Up, Row 2 = -Forward (in standard view matrix)
    glm::vec3 cameraRight = glm::vec3(view[0][0], view[1][0], view[2][0]);
    glm::vec3 cameraUp    = glm::vec3(view[0][1], view[1][1], view[2][1]);
    glm::vec3 cameraFwd   = -glm::vec3(view[0][2], view[1][2], view[2][2]);

    // Reconstruct Camera Rotation Quaternion (Used to align ducks)
    // Columns: Right, Up, Backward (-Forward)
    glm::mat3 camRotMat(cameraRight, cameraUp, -cameraFwd);
    glm::quat camRot = glm::quat_cast(camRotMat);

    // --- DUCK SPAWNING LOGIC ---
    static float spawnTimer = 0.0f;
    const float SPAWN_INTERVAL = 2.0f;

    spawnTimer += deltaTime;
    if (spawnTimer >= SPAWN_INTERVAL) {
        spawnTimer = 0.0f;

        // Determine spawn side: 50% Left (-1), 50% Right (+1) relative to camera
        bool spawnLeft = (rand() % 2) == 0;
        float sideMultiplier = spawnLeft ? -1.0f : 1.0f;

        float distForward = 15.0f;  // Distance in front of camera
        float distSide = 35.0f;     // How far to the left/right
        float randUp = 0.0f + (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 6.0f;

        // Position = CameraPos + (Forward * depth) + (Right * side) + (Up * height)
        glm::vec3 spawnPos = worldContext->camera->position
                           + (cameraFwd * distForward)
                           + (cameraRight * (distSide * sideMultiplier))
                           + (cameraUp * randUp);

        // Spawn the duck
        DuckEntity& newDuck = worldContext->EntityManager.CreateDuckEntity(*worldContext, spawnPos);

        if (newDuck.hasComponent<Velocity>() && newDuck.hasComponent<Transform>()) {
            float speed = 2.0f;

            newDuck.getComponent<Transform>().rotation = camRot;

            if (!spawnLeft) {
                 newDuck.getComponent<Transform>().WorldRotate(180.0f, glm::vec3(0,1,0));
            }

            newDuck.getComponent<Velocity>().setVelocity(glm::vec3(1.0f, 0.0f, 0.0f), speed);
        }

        std::cout << "Spawned duck at relative pos: " << spawnPos.x << ", " << spawnPos.y << ", " << spawnPos.z << std::endl;
    }

    // Update all entities in the world
    worldContext->EntityManager.Update(deltaTime);

    // Handle Player Shooting/Raycasting
    auto raySourceEntities = worldContext->EntityManager.GetEntitiesWith<RaycastSource, Transform>();

    if (!raySourceEntities.empty()) {
        Entity* playerEntity = raySourceEntities[0];
        auto& transform = playerEntity->getComponent<Transform>();
        auto& raySource = playerEntity->getComponent<RaycastSource>();

        // Offset weapon position
        transform.position = worldContext->camera->position
                           + (cameraFwd * 0.5f)
                           - (cameraUp * 0.2f);

        // Check Input
        if (InputManager::isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {

            glm::vec3 rayDir = InputUtils::screenToWorldRay(
                InputManager::getMousePosition(),
                InputManager::getWindowWidth(),
                InputManager::getWindowHeight(),
                *worldContext->camera
            );

            raySource.direction = rayDir;
            raySource.drawRay = true;

            if (worldContext->collisionSystem) {
                // Capture the result
                auto result = worldContext->collisionSystem->RaycastFromEntity(worldContext->EntityManager, *playerEntity);

                // Print the hit entity
                if (result.hit && result.hitEntity) {

                        DuckEntity* hitDuck = dynamic_cast<DuckEntity*>(result.hitEntity);

                        if (hitDuck) {
                            hitDuck->KillDuck();
                        } else {
                            std::cout << "Hit something, but it wasn't a duck" << std::endl;
                        }

                } else {
                    std::cout << "MISS!" << std::endl;
                }
            }
        }
    }
}

void GameStateManager::updatePaused(float deltaTime) {
    // Paused logic
}

void GameStateManager::updateRoundTransition(float deltaTime) {
    if (stateTimer > 2.0f) {
        std::cout << "[GameStateManager] Round transition complete" << std::endl;
        setState(GameState::PLAYING);
    }
}

void GameStateManager::updateGameOver(float deltaTime) {
    // Game Over logic
}

void GameStateManager::updateOptions(float deltaTime) {
    // Options logic
}

// ============================================================================
// STATE-SPECIFIC RENDER METHODS
// ============================================================================

void GameStateManager::renderMenu() { }

void GameStateManager::renderPlaying() { }

void GameStateManager::renderPaused() {
    renderPlaying();
}

void GameStateManager::renderRoundTransition() { }

void GameStateManager::renderGameOver() { }

void GameStateManager::renderOptions() { }