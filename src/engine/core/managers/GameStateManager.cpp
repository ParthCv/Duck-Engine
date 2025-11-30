#include <glad/glad.h>
#include "GameStateManager.h"
#include <iostream>

#include "../../ecs/World.h"
#include "../../ecs/Component.h"
#include "../../utils/InputUtils.h"
#include "../../system/CollisionSystem.h"
#include "../../game/DuckFactory.h"
#include "../../system/LifecycleSystem.h"
#include "InputManager.h"
#include "AudioManager.h"
#include "../../game/DuckGameState.h"

GameStateManager::GameStateManager()
    : currentState(GameState::MENU)
    , previousState(GameState::MENU)
    , stateTimer(0.0f)
    , onStateChangeCallback(nullptr)
    , worldContext(nullptr)
{

    std::cout << "[GameStateManager] Initialized..." << std::endl;
    std::cout << "[GameStateManager] Initialized - Starting in MENU state" << std::endl;
}

void GameStateManager::setState(GameState newState) {
    if (newState == currentState) return;

    previousState = currentState;
    currentState = newState;
    stateTimer = 0.0f;

    // === HANDLE CURSOR VISIBILITY BASED ON STATE (HIDE DURING PLAYING) ===
    if (currentState == GameState::PLAYING) {
        // Lock/Hide cursor for FPS gameplay
        InputManager::setCursorVisible(false);
        InputManager::resetMouseDelta(); // Prevent camera jump
    } else {
        // Unlock/Show cursor for menus
        InputManager::setCursorVisible(true);
    }

    notifyStateChange(previousState, currentState);
}

void GameStateManager::update(float deltaTime) {
    stateTimer += deltaTime;

    // Call state-specific update
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
    // Call state-specific render
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
    // Note: Actual game reset logic should be handled by GameManager
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
    if (!worldContext ) {
        return;
    }

        // --- 1. HANDLE CAMERA ROTATION (FPS STYLE) ---
    glm::vec2 mouseDelta = InputManager::getMouseDelta();
    // Only rotate if there was movement
    if (glm::length(mouseDelta) > 0.001f) {
        worldContext->camera->processMouseMovement(mouseDelta.x, mouseDelta.y);
    }

    // --- 2. RETRIEVE UPDATED CAMERA VECTORS ---
    glm::vec3 cameraFwd   = worldContext->camera->front;
    glm::vec3 cameraRight = worldContext->camera->right;
    glm::vec3 cameraUp    = worldContext->camera->up;

    // Reconstruct Camera Rotation Quaternion (Used to align ducks)
    glm::mat3 camRotMat(cameraRight, cameraUp, -cameraFwd);
    glm::quat camRot = glm::quat_cast(camRotMat);

    // --- 3. DUCK SPAWNING LOGIC ---
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

        Entity* newDuck = DuckFactory::createDuck(*worldContext, spawnPos, DuckGameState::get().getDuckSpeedBasedOnRound());
        DuckGameState::get().decrementNumOfDucks();
        if (newDuck->hasComponent<Velocity>() && newDuck->hasComponent<Transform>()) {
            auto& transform = newDuck->getComponent<Transform>();
            transform.rotation = camRot;

            if (!spawnLeft) {
                transform.rotation = glm::quat_cast(glm::rotate(glm::mat4_cast(transform.rotation), glm::radians(180.0f), glm::vec3(0,1,0)));
            }

            auto& velocity = newDuck->getComponent<Velocity>();
            velocity.Direction = glm::vec3(1.0f, 0.0f, 0.0f);
        }
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
        if (
            InputManager::isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT) &&
            DuckGameState::get().hasBulletsRemaining()
        ) {
            AudioManager::Get().PlaySound("shoot", 0.5f);

            // FPS STYLE: Raycast ALWAYS goes straight forward from camera
            glm::vec3 rayDir = cameraFwd;

            raySource.direction = rayDir;
            raySource.drawRay = true;

            if (worldContext->collisionSystem) {
                // Capture the result
                auto result = worldContext->collisionSystem->RaycastFromEntity(worldContext->EntityManager, *playerEntity);

                // Print the hit entity
                if (result.hit && result.hitEntity) {

                        if (result.hitEntity->hasComponent<DuckComponent>()) {
                            LifecycleSystem::killDuck(*result.hitEntity);
                        } else {
                            std::cout << "Hit something, but it wasn't a duck" << std::endl;
                        }

                } else {
                    std::cout << "MISS!" << std::endl;
                }
            }
            DuckGameState::get().decrementBullet();
        }
    }
}

void GameStateManager::updatePaused(float deltaTime) {
    // TODO: Paused state - no game updates
    // - Can still update UI animations
    // - Check for resume/quit inputs
}

void GameStateManager::updateRoundTransition(float deltaTime) {
    // TODO: Between-round logic
    // - Display round results
    // - Show "Get Ready" message
    // - Auto-transition to next round after delay

    // Example: Auto-transition after 2 seconds
    if (stateTimer > 2.0f) {
        std::cout << "[GameStateManager] Round transition complete" << std::endl;
        setState(GameState::PLAYING);
    }
}

void GameStateManager::updateGameOver(float deltaTime) {
    // TODO: Game over state
    // - Display final score
    // - Check for restart/menu inputs
    // - High score logic (if implemented)
}

void GameStateManager::updateOptions(float deltaTime) {
    // TODO: Options menu logic
    // - Volume sliders
}

// ============================================================================
// STATE-SPECIFIC RENDER METHODS
// ============================================================================

void GameStateManager::renderMenu() {
    // TODO: Render main menu
    // - Title
    // - Play button
    // - Options button
    // - Quit button

}

void GameStateManager::renderPlaying() {
    // TODO: Render active gameplay
    // - Ducks
    // - Crosshair
    // - Score/HUD
    // - Background

    // This will call your rendering systems
}

void GameStateManager::renderPaused() {
    // TODO: Render game (dimmed/frozen) + pause overlay
    // - "PAUSED" text
    // - Resume button
    // - Quit to menu button

    // Might want to render the game scene behind a semi-transparent overlay
    renderPlaying(); // Render game beneath
    // Then render pause overlay on top
}

void GameStateManager::renderRoundTransition() {
    // TODO: Render transition screen
    // - "Round X Complete!"
    // - Score summary
    // - "Get Ready..."
}

void GameStateManager::renderGameOver() {
    // TODO:  game over screen
    // - "Game Over"
    // - Final score
    // - High score (if beat)
    // - Restart button
    // - Menu button
}

void GameStateManager::renderOptions() {
    // TODO: Render options menu
    // - Volume controls
    // - Back button
}
