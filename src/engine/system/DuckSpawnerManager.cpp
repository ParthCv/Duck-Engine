//
// Created by User on 11/25/2025.
//

#include "DuckSpawnerManager.h"
#include "../game/GameUtils.h"
#include "../game/DuckGameState.h"
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
        !bSpawned && DuckGameState::get().getNumOfDucks() > 0)
    {
        std::cout << static_cast<int>(accumulatedTime) << "\n";
        SpawnDuck();
        bSpawned = true;
        // numberOfDucksToSpawn--;
        DuckGameState::get().decrementNumOfDucks();
    }

    if (static_cast<int>(accumulatedTime) % static_cast<int>(spawnInterval) != 0) {
        bSpawned = false;
    }

    // if (numberOfDucksToSpawn <= 0) {
    if (DuckGameState::get().isRoundComplete()) {
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
    world->EntityManager.CreateDuckEntityWithVelocity(*world, spawnPositions[randomIndex], DuckGameState::get().getDuckSpeedBasedOnRound());
}

void DuckSpawnerManager::ResetRound() {
    // numberOfDucksToSpawn = ducksPerRound;
    DuckGameState::get().startNextRound();
    // AudioManager::Get().PlaySound("win", 0.5f);
}

void DuckSpawnerManager::SetDucksPerRound(int num) {
    // ducksPerRound = num;
    DuckGameState::get().setMaxNumOfDucks(num);
}
