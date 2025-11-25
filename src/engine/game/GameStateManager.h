//
// Created by Norm on 11/17/2025.
//

#pragma once
#include <string>
#include <functional>
#include "../input/InputManager.h"

class World;

// Possible game states for Duck Hunt (TBD)
enum class GameState {
    MENU,               // Main menu
    PLAYING,            // Active gameplay
    PAUSED,             // Game paused
    ROUND_TRANSITION,   // Between rounds? Multiple rounds per game?
    GAME_OVER,          // All rounds complete or failed
    OPTIONS             // Settings menu (volume)
};

class GameStateManager {
public:
    GameStateManager();

    // State management
    void setState(GameState newState);
    GameState getCurrentState() const { return currentState; }
    GameState getPreviousState() const { return previousState; }

    // Update and render for current state
    void update(float deltaTime);
    void render();

    // State queries
    bool isPlaying() const { return currentState == GameState::PLAYING; }
    bool isPaused() const { return currentState == GameState::PAUSED; }
    bool isInMenu() const { return currentState == GameState::MENU; }
    bool isGameOver() const { return currentState == GameState::GAME_OVER; }

    // Utility methods
    void togglePause();
    void resumeGame();
    void restartGame();
    void returnToMenu();

    void setWorldContext(World* world) { worldContext = world; }

    // Get state as string (for debugging)
    std::string getStateString() const;

    // Optional: Register callbacks for state changes
    using StateCallback = std::function<void(GameState oldState, GameState newState)>;
    void setOnStateChange(StateCallback callback) { onStateChangeCallback = callback; }

private:
    GameState currentState;
    GameState previousState;

    // Time tracking for state-specific logic
    float stateTimer; // duration in current state

    World* worldContext = nullptr;

    // State-specific update methods
    void updateMenu(float deltaTime);
    void updatePlaying(float deltaTime);
    void updatePaused(float deltaTime);
    void updateRoundTransition(float deltaTime);
    void updateGameOver(float deltaTime);
    void updateOptions(float deltaTime);

    // State-specific render methods
    void renderMenu();
    void renderPlaying();
    void renderPaused();
    void renderRoundTransition();
    void renderGameOver();
    void renderOptions();

    // Callback for state changes
    StateCallback onStateChangeCallback;

    // Helper to trigger callbacks
    void notifyStateChange(GameState oldState, GameState newState);
};