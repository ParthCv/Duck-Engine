#include "DuckSpawnerManager.h"
#include "../src/engine/game/GameUtils.h"
#include "../src/engine/game/ecs/system/GameStateSystem.h"
#include "../src/engine/core/managers/AudioManager.h"
#include "../src/engine/game/DuckFactory.h"

#include <iostream>

DuckSpawnerManager::DuckSpawnerManager(World &InWorld) :
    world(&InWorld)
{
    spawnPositions = GenerateHalfRingPoints(glm::vec3(0,0,0), 15, 5);
}

void DuckSpawnerManager::Update(float deltaTime) {
    // Get game state entity for round management
    Entity* gameState = world->getGameStateEntity();
    if (!gameState) return;

    accumulatedTime += deltaTime;
    if (static_cast<int>(accumulatedTime) % static_cast<int>(spawnInterval) == 0 &&
        !bSpawned && GameStateSystem::canSpawnMoreDucks(*gameState))
    {
        std::cout << static_cast<int>(accumulatedTime) << "\n";
        SpawnDuck();
        bSpawned = true;
    }

    if (static_cast<int>(accumulatedTime) % static_cast<int>(spawnInterval) != 0) {
        bSpawned = false;
    }

    if (GameStateSystem::isRoundComplete(*gameState)) {
        if (GameStateSystem::isRoundFailed(*gameState)) {
            GameStateSystem::endGameDefeat(*gameState);
        } else {
            ResetRound();
        }
    }
}

void DuckSpawnerManager::SpawnDuck()
{
    // Get game state entity for duck speed and spawn tracking
    Entity* gameState = world->getGameStateEntity();
    if (!gameState) return;


    // TODO: Half-ring Solution
    int randomIndex = rand() % spawnPositions.size();
    DuckFactory::createDuck(*world, spawnPositions[randomIndex], GameStateSystem::getDuckSpeed(*gameState));

    GameStateSystem::spawnDuck(*gameState);  // Just marks UI slot as spawned
    AudioManager::Get().PlaySound("quack", 1.0f);
}

void DuckSpawnerManager::ResetRound() {
    // Get game state entity for round reset
    Entity* gameState = world->getGameStateEntity();
    if (!gameState) return;

    GameStateSystem::startNextRound(*gameState);
}

void DuckSpawnerManager::SetDucksPerRound(int num) {
    // Get game state entity for duck count setting
    Entity* gameState = world->getGameStateEntity();
    if (!gameState) return;

    GameStateSystem::setMaxNumOfDucks(*gameState, num);
}
