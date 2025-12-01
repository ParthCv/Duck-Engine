#pragma once

class World;

class DuckSpawnerManager {
public:
    DuckSpawnerManager(World& world);

    void Update(float deltaTime);

private:
    World& world;

    // Spawn timing
    float spawnTimer = 0.0f;
    float spawnInterval = 4.0f;

    void spawnDuck();
};