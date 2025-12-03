#pragma once

enum class DuckState {
    NOT_SPAWNED,
    SPAWNED,
    HIT,
    ESCAPED
};

// Pure data component to track round state for a game entity
// Logic should be handled by systems, not components
struct GameRoundComponent {
    int currentRound = 1;
    int maxDucksPerRound = 10;
    int ducksSpawned = 0;
    int ducksResolved = 0;
    int ducksEscaped = 0;
    int maxDucksEscapedAllowed = 3;

    float duckSpeedMultiplier = 1.1f;
    float initialDuckSpeed = 0.5f;
};

// Pure data component for ammo tracking
// Systems should check 'current > 0' and modify values directly
struct AmmoComponent {
    int current = 10;
    int max = 10;
};

// Pure data component for score tracking
// Point calculation should be done by a scoring system
struct ScoreComponent {
    int totalScore = 0;
    int basePointsPerDuck = 25;
    float scoreMultiplier = 1.5f;
};

// Pure data component for UI duck state tracking
// Systems should access states array directly with bounds checking
struct DuckUIStateComponent {
    static constexpr int MAX_DUCKS = 10;
    DuckState states[MAX_DUCKS] = {};
};
