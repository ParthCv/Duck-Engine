//
// Created by User on 11/25/2025.
//

#include "DuckSpawnerManager.h"

DuckSpawnerManager::DuckSpawnerManager(World &InWorld) :
    world(&InWorld)
{
}

void DuckSpawnerManager::Update(float deltaTime)
{
    accumulatedTime += deltaTime;
    if (static_cast<int>(accumulatedTime) % static_cast<int>(spawnInterval) == 0)
    {
        SpawnDuck();
    }
}

void DuckSpawnerManager::SpawnDuck()
{
    glm::vec3 cameraPositionOffset = world->camera->position + glm::vec3(0.0f, 0.0f, distanceFromCamera);
    world->EntityManager.CreateDuckEntity(*world, cameraPositionOffset);
}