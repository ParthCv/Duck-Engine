#pragma once
#include <vector>

class World;
class Entity;
class EventQueue;

// Proper system to manage game states respecting ECS Patterns.
class GameStateSystem {
public:

    // === Entity Lookup ===
    static Entity* getGameStateEntity(World& world);


    // === Round Management ===
    static bool canSpawnMoreDucks(Entity& gameState);
    static bool isRoundComplete(Entity& gameState);
    static bool isRoundFailed(Entity& gameState);
    static float getDuckSpeed(Entity& gameState);
    static void startNextRound(Entity& gameState);
    static void endGameDefeat(Entity& gameState);
    static void endGameVictory(Entity& gameState);


    // === Ammo Management ===
    static bool hasAmmo(Entity& gameState);
    static void consumeAmmo(Entity& gameState);
    static void refillAmmo(Entity& gameState);


    // === Duck Tracking ===
    static void spawnDuck(Entity& gameState);
    static void hitDuck(Entity& gameState);
    static void duckEscaped(Entity& gameState);


    // === Getters ===
    static int getRound(Entity& gameState);
    static int getScore(Entity& gameState);
    static int getNumOfBullets(Entity& gameState);
    static int getMaxNumOfBullets(Entity& gameState);
    static int getMaxNumOfDucks(Entity& gameState);
    static int getNumOfDucksEscaped(Entity& gameState);
    static int getDuckSpawnIndex(Entity& gameState);
    static int getDuckResolveIndex(Entity& gameState);


    // === Setters ===
    static void setMaxNumOfDucks(Entity& gameState, int num);
    static void resetGameState(Entity& gameState);


    // === Duck State UI ===
    static int getDuckStateAtIndex(Entity& gameState, int index);
    static void resetDuckStates(Entity& gameState);


    // === Event Management ===
    template<typename T>
    static const std::vector<T>& getEvents();

    template<typename T>
    static bool hasEvents();

    static void clearEvents();
};
