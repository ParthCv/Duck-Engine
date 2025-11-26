//
// Created by User on 11/25/2025.
//

#include "DuckSpawnerManager.h"

#include <iostream>

DuckSpawnerManager::DuckSpawnerManager(World &InWorld) :
    world(&InWorld)
{
}

void DuckSpawnerManager::Update(float deltaTime)
{
    accumulatedTime += deltaTime;
    if (static_cast<int>(accumulatedTime) % static_cast<int>(spawnInterval) == 0 && !bSpawned && numberOfDucksToSpawn > 0)
    {
        std::cout << static_cast<int>(accumulatedTime) << "\n";
        SpawnDuck();
        bSpawned = true;
        numberOfDucksToSpawn--;
    }

    if (static_cast<int>(accumulatedTime) % static_cast<int>(spawnInterval) != 0) {
        bSpawned = false;
    }

    if (numberOfDucksToSpawn <= 0) {
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
}

void DuckSpawnerManager::ResetRound() {
    numberOfDucksToSpawn = ducksPerRound;
}

void DuckSpawnerManager::SetDucksPerRound(int num) {
    ducksPerRound = num;
}
