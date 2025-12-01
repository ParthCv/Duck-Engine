#pragma once
#include <cmath>

#include "AudioManager.h"
#include "../../game/EventQueue.h"

enum class DuckState {
    NOT_SPAWNED,
    SPAWNED,
    HIT,
    ESCAPED
};

class GameStateManager {
    static GameStateManager* instance;

    int round = 1;
    int score = 0;
    int maxNumOfBullets = 10;
    int numOfBullets = 10;
    int maxNumOfDucks = 10;

    // Two separate counters for UI tracking
    int duckSpawnIndex = 0;     // Next slot to mark as SPAWNED (0-9)
    int duckResolveIndex = 0;   // Next slot to mark as HIT/ESCAPED (0-9)

    int numOfDucksEscaped = 0;
    int numOfDucksEscapedAllowed = 3;

    DuckState duckStates[10] = {};

    int basePointsPerDuck = 25;
    float initialSpeed = 0.5;
    const float duckSpeedMultiplier = 1.1f;
    const float scoreMultiplier = 1.1f;

    EventQueue events;

    // Private mutators
    void incrementRound() {
        round++;
    }

    void setRound(int r) {
        round = r;
    }

    void addScore(int points) {
        score += points;
    }

    void setScore(int s) {
        score = s;
    }

    void decrementBullet() {
        if (numOfBullets > 0) {
            numOfBullets--;
        }
    }

    void setBullets(int bullets) {
        numOfBullets = bullets;
    }

    void incrementDucksEscaped() {
        numOfDucksEscaped++;
    }

    void setDucksEscaped(int escaped) {
        numOfDucksEscaped = escaped;
    }

public:
    static GameStateManager& get() {
        if (!instance)
            instance = new GameStateManager();
        return *instance;
    }

    // Getters
    int getRound() const { return round; }
    int getScore() const { return score; }
    int getNumOfBullets() const { return numOfBullets; }
    int getMaxNumOfBullets() const { return maxNumOfBullets; }
    int getMaxNumOfDucks() const { return maxNumOfDucks; }
    int getNumOfDucksEscaped() const { return numOfDucksEscaped; }
    int getDuckSpawnIndex() const { return duckSpawnIndex; }
    int getDuckResolveIndex() const { return duckResolveIndex; }

    bool hasBulletsRemaining() const { return numOfBullets > 0; }

    // Can we spawn more ducks this round?
    bool canSpawnMoreDucks() const {
        return duckSpawnIndex < maxNumOfDucks;
    }

    // Is the round over? (all ducks spawned AND all resolved)
    bool isRoundComplete() const {
        return duckSpawnIndex >= maxNumOfDucks && duckResolveIndex >= maxNumOfDucks;
    }

    bool isRoundFailed() const {
        return isRoundComplete() && numOfDucksEscaped > numOfDucksEscapedAllowed;
    }

    float getDuckSpeedBasedOnRound() const {
        return initialSpeed * std::pow(duckSpeedMultiplier, round - 1);
    }

    EventQueue& getEvents() { return events; }
    void clearEvents() { events.clear(); }

    // === High-Level Actions ===

    void shootBullet() {
        if (hasBulletsRemaining()) {
            decrementBullet();
            events.emit(BulletFiredEvent{numOfBullets});
        }
    }

    /**
     * Call when spawning a duck
     * Marks the next UI slot as SPAWNED and increments the spawn counter
     */
    void spawnDuck() {
        if (canSpawnMoreDucks()) {
            duckStates[duckSpawnIndex] = DuckState::SPAWNED;
            duckSpawnIndex++;
        }
    }

    /**
     * Call when a duck is hit
     * Marks the next unresolved UI slot as HIT
     */
    void hitDuck() {
        if (duckResolveIndex < maxNumOfDucks) {
            int points = static_cast<int>(basePointsPerDuck * std::pow(scoreMultiplier, round - 1));
            addScore(points);

            duckStates[duckResolveIndex] = DuckState::HIT;

            events.emit(DuckShotEvent{duckResolveIndex, points});

            duckResolveIndex++;
        }
    }

    /**
     * Call when a duck escapes
     * Marks the next unresolved UI slot as ESCAPED
     */
    void duckEscaped() {
        if (duckResolveIndex < maxNumOfDucks) {
            incrementDucksEscaped();

            if (numOfDucksEscaped >= numOfDucksEscapedAllowed) {
                resetGame();
            }

            duckStates[duckResolveIndex] = DuckState::ESCAPED;
            AudioManager::Get().PlaySound("flapping", 1.0f);
            events.emit(DuckEscapedEvent{duckResolveIndex});

            duckResolveIndex++;
        }
    }

    void startNextRound() {
        incrementRound();
        setBullets(maxNumOfBullets);
        setDucksEscaped(0);
        duckSpawnIndex = 0;
        duckResolveIndex = 0;
        resetDuckStates();
        AudioManager::Get().PlaySound("win", 0.8f);
        events.emit(RoundStartEvent{round, maxNumOfDucks});
    }

    void resetGame() {
        setRound(1);
        setScore(0);
        setBullets(maxNumOfBullets);
        setDucksEscaped(0);
        duckSpawnIndex = 0;
        duckResolveIndex = 0;
        resetDuckStates();
        AudioManager::Get().PlaySound("lose", 0.8f);
        events.emit(GameOverEvent{false, 0, 0});
        events.emit(StartGameEvent{});
    }

    void endGameVictory() {
        events.emit(GameOverEvent{true, score, round});
    }

    void endGameDefeat() {
        events.emit(GameOverEvent{false, score, round});
    }

    void setMaxNumOfBullets(int max) {
        maxNumOfBullets = max;
        if (numOfBullets > max) {
            numOfBullets = max;
        }
    }

    void setMaxNumOfDucks(int max) {
        maxNumOfDucks = max;
    }

    DuckState getDuckStateAtIndex(int index) const {
        if (index >= 0 && index < maxNumOfDucks) {
            return duckStates[index];
        }
        return DuckState::NOT_SPAWNED;
    }

    void resetDuckStates() {
        for (int i = 0; i < maxNumOfDucks; i++) {
            duckStates[i] = DuckState::NOT_SPAWNED;
        }
    }
};