#pragma once
#include <cmath>

enum class DuckState {
    NOT_SPAWNED,
    SPAWNED,
    HIT,
    ESCAPED
};

// Component to track round state for a game entity
struct GameRoundComponent {
    int currentRound = 1;
    int maxDucksPerRound = 10;
    int ducksSpawned = 0;
    int ducksResolved = 0;
    int ducksEscaped = 0;
    
    float duckSpeedMultiplier = 1.1f;
    float initialDuckSpeed = 0.5f;
    
    bool isRoundComplete() const {
        return ducksResolved >= maxDucksPerRound;
    }
    
    bool hasRoundFailed() const {
        return ducksEscaped > 3;
    }
    
    float getDuckSpeed() const {
        return initialDuckSpeed * std::pow(duckSpeedMultiplier, currentRound - 1);
    }
};

struct AmmoComponent {
    int current = 10;
    int max = 10;
    
    bool hasAmmo() const { 
        return current > 0; 
    }
    
    void consume() { 
        if (current > 0) current--; 
    }
    
    void refill() { 
        current = max; 
    }
};

struct ScoreComponent {
    int totalScore = 0;
    int basePointsPerDuck = 25;
    float scoreMultiplier = 1.5f;
    
    int calculateDuckPoints(int round) const {
        return static_cast<int>(basePointsPerDuck * std::pow(scoreMultiplier, round - 1));
    }
};

struct DuckUIStateComponent {
    DuckState states[10] = {};
    
    void resetStates() {
        for (int i = 0; i < 10; i++) {
            states[i] = DuckState::NOT_SPAWNED;
        }
    }
    
    DuckState getStateAtIndex(int index) const {
        if (index >= 0 && index < 10) {
            return states[index];
        }
        return DuckState::NOT_SPAWNED;
    }
};
