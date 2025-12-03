#pragma once

enum class DuckState {
    NOT_SPAWNED,
    SPAWNED,
    HIT,
    ESCAPED
};

struct GameRoundComponent {
    int currentRound = 1;
    int maxDucksPerRound = 10;

    // Two separate counters for UI tracking
    int duckSpawnIndex = 0;     // Next slot to mark as SPAWNED (0-9)
    int duckResolveIndex = 0;   // Next slot to mark as HIT/ESCAPED (0-9)

    int ducksEscaped = 0;
    int maxDucksEscapedAllowed = 3;

    float duckSpeedMultiplier = 1.1f;
    float initialDuckSpeed = 0.5f;
};

struct AmmoComponent {
    int current = 10;
    int max = 10;
};

struct ScoreComponent {
    int totalScore = 0;
    int basePointsPerDuck = 25;
    float scoreMultiplier = 1.5f;
};

struct DuckUIStateComponent {
    static constexpr int MAX_DUCKS = 10;
    DuckState states[MAX_DUCKS] = {};
};
