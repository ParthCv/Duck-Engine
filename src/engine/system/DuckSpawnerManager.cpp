#include "DuckSpawnerManager.h"
#include "../game/GameUtils.h"
#include "../core/managers/GameStateManager.h"
#include "../core/managers/AudioManager.h"

#include <iostream>

DuckSpawnerManager::DuckSpawnerManager(World &InWorld) :
    world(&InWorld)
{
    spawnPositions = GenerateHalfRingPoints(glm::vec3(0,0,0), 15, 5);
}

void DuckSpawnerManager::Update(float deltaTime) {
    accumulatedTime += deltaTime;
    if (static_cast<int>(accumulatedTime) % static_cast<int>(spawnInterval) == 0 &&
        !bSpawned && GameStateManager::get().getNumOfDucks() > 0)
    {
        std::cout << static_cast<int>(accumulatedTime) << "\n";
        SpawnDuck();
        bSpawned = true;
        // numberOfDucksToSpawn--;
        GameStateManager::get().decrementNumOfDucks();
    }

    if (static_cast<int>(accumulatedTime) % static_cast<int>(spawnInterval) != 0) {
        bSpawned = false;
    }

    // if (numberOfDucksToSpawn <= 0) {
    if (GameStateManager::get().isRoundComplete()) {
        ResetRound();
    }
}

void DuckSpawnerManager::SpawnDuck()
{
    // TODO: Invert the Camera's current position and normalize to get the forward vector in local space.
    // glm::vec3 directionVector = glm::normalize(-world->camera->position) * distanceFromCamera;
    // glm::vec3 cameraPositionOffset = world->camera->position + directionVector;
    // world->EntityManager.CreateDuckEntity(*world, cameraPositionOffset);

    // glm::vec3 initialForwardVector = glm::vec3(0.0f, 0.0f, 1.0f);
    // glm::vec3 currentForwardVector = world.camera

    // TODO: Half-ring Solution
    int randomIndex = rand() % spawnPositions.size();
    // world->EntityManager.CreateDuckEntity(*world, spawnPositions[randomIndex]);
    world->EntityManager.CreateDuckEntityWithVelocity(*world, spawnPositions[randomIndex], GameStateManager::get().getDuckSpeedBasedOnRound());
}

void DuckSpawnerManager::ResetRound() {
    // numberOfDucksToSpawn = ducksPerRound;
    GameStateManager::get().startNextRound();
    // AudioManager::Get().PlaySound("win", 0.5f);
}

void DuckSpawnerManager::SetDucksPerRound(int num) {
    // ducksPerRound = num;
    GameStateManager::get().setMaxNumOfDucks(num);
}
