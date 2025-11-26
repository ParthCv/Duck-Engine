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

    float spawnInterval = 3.0f;

    float distanceFromCamera = 10;

    float accumulatedTime = 0.0f;

    DuckSpawnerManager(World& InWorld);

    void Update(float deltaTime);

    void SpawnDuck();
};


#endif //DUCKENGINE_DUCKSPAWNERMANAGER_H