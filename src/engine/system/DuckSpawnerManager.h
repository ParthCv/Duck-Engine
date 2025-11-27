//
// Created by User on 11/25/2025.
//

#ifndef DUCKENGINE_DUCKSPAWNERMANAGER_H
#define DUCKENGINE_DUCKSPAWNERMANAGER_H
#include "../ecs/World.h"
#include "../ecs/Entity.h"

class World;

class DuckSpawnerManager {
public:
    World* world;

    bool bSpawned = false;

    float spawnInterval = 2.0f;

    float distanceFromCamera = 5;

    int numberOfDucksToSpawn = 5;

    int ducksPerRound = 5;

    float accumulatedTime = 0.0f;

    std::vector<glm::vec3> spawnPositions;

    DuckSpawnerManager(World& InWorld);

    void Update(float deltaTime);

    void SpawnDuck();

    void ResetRound();

    void SetDucksPerRound(int num);
};


#endif //DUCKENGINE_DUCKSPAWNERMANAGER_H