#include "GameStateManager.h"
#include <iostream>

GameStateManager::GameStateManager()
    : currentState(GameState::MENU)
    , previousState(GameState::MENU)
    , stateTimer(0.0f)
    , onStateChangeCallback(nullptr)
{
    std::cout << "[GameStateManager] Initialized - Starting in MENU state" << std::endl;
}

void GameStateManager::setState(GameState newState) {
    if (newState == currentState) {
        return; // Already in this state
    }

    std::cout << "[GameStateManager] State transition: "
              << getStateString() << " -> ";

    previousState = currentState;
    currentState = newState;
    stateTimer = 0.0f; // Reset timer for new state

    std::cout << getStateString() << std::endl;

    // Notify callback if registered
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
    // - Check for button clicks
    // - Handle menu navigation
    // - Start game when "Play" is clicked

    // Debug: Auto-transition after 3 seconds (remove this later)
    // if (stateTimer > 3.0f) {
    //     setState(GameState::PLAYING);
    // }
}

void GameStateManager::updatePlaying(float deltaTime) {
    // TODO: Active gameplay logic here
    // - Update ducks
    // - Check for shots
    // - Update score
    // - Check win/lose conditions

    // This is where your main game loop logic goes
    // The actual duck/shooting logic will be in GameManager
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
