//
// Created by Nathan on 2025-11-26.
//
#pragma once

class DuckGameState {
    static DuckGameState* instance;

    int round = 1;              // Start at round 1
    int score = 0;
    int maxNumOfBullets = 10;
    int numOfBullets = 10;
    int maxNumOfDucks = 10;     // Num of ducks per round
    int numOfDucks = 10;        // Ducks left to spawn during a round
    int numOfDucksEscaped = 0;  // Ducks we did not shoot and disappeared

    int basePointsPerDuck = 25;
    float initialSpeed = 0.5;
    const float duckSpeedMultiplier = 1.1f; // Increases per round
    const float scoreMultiplier = 1.1f; // Increases per round

    // EventQueue if time permits

public:
    static DuckGameState& get() {
        if (!instance)
            instance = new DuckGameState();
        return *instance;
    }

    // === Round Management ===
    void incrementRound() {
        round++;
    }

    void resetRound() {
        round = 1;
    }

    int getRound() const {
        return round;
    }

    // === Score Management ===
    void addScore(int points) {
        score += points;
    }

    void resetScore() {
        score = 0;
    }

    int getScore() const {
        return score;
    }

    // === Bullet Management ===
    void decrementBullet() {
        if (numOfBullets > 0) {
            numOfBullets--;
        }
    }

    void resetBullets() {
        numOfBullets = maxNumOfBullets;
    }

    int getNumOfBullets() const {
        return numOfBullets;
    }

    int getMaxNumOfBullets() const {
        return maxNumOfBullets;
    }

    void setMaxNumOfBullets(int max) {
        maxNumOfBullets = max;
        if (numOfBullets > max) {
            numOfBullets = max;
        }
    }

    bool hasBulletsRemaining() const {
        return numOfBullets > 0;
    }

    // === Duck Management ===

    int getNumOfDucks() const {
        return numOfDucks;
    }

    void decrementNumOfDucks() {
        if (numOfDucks > 0) {
            numOfDucks--;
        }
    }

    void incrementDucksEscaped() {
        numOfDucksEscaped++;
        decrementNumOfDucks();
    }

    void resetDucks() {
        numOfDucks = maxNumOfDucks;
        numOfDucksEscaped = 0;
    }

    int getMaxNumOfDucks() const {
        return maxNumOfDucks;
    }

    void setMaxNumOfDucks(int max) {
        maxNumOfDucks = max;
        if (numOfDucks > max) {
            numOfDucks = max;
        }
    }

    int getNumOfDucksEscaped() const {
        return numOfDucksEscaped;
    }

    bool areDucksRemaining() const {
        return numOfDucks > 0;
    }

    void hitDuck() {
        addScore(static_cast<int>(basePointsPerDuck * scoreMultiplier * round));
        decrementNumOfDucks();
    }

    void shootBullet() {
        if (hasBulletsRemaining()) {
            decrementBullet();
        }
    }

    void duckEscaped() {
        incrementDucksEscaped();
    }

    float getDuckSpeedBasedOnRound() {
        return initialSpeed * duckSpeedMultiplier * round;
    }

    void startNextRound() {
        incrementRound();
        resetBullets();
        resetDucks();
    }

    void resetGame() {
        resetRound();
        resetScore();
        resetBullets();
        resetDucks();
    }

    // === Game state queries ===
    bool isRoundComplete() const {
        return numOfDucks <= 0;
    }

    /**
     * Returns whether we failed a round.
     *
     * We fail a round if we hit less than 7 ducks.
     * Ensure we call this after the last duck has either been shot or escaped
     * @return
     */
    bool isRoundFailed() const {
        return numOfDucks == 0 &&
            numOfDucksEscaped <= 3;  // We don't hit at least 7
    }
};