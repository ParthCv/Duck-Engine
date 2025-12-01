#include "DuckSpawnerManager.h"
#include "../src/engine/ecs/World.h"
#include "../src/engine/ecs/Component.h"
#include "../src/engine/ecs/components/GameRoundComponent.h"
#include "../src/engine/game/DuckFactory.h"

DuckSpawnerManager::DuckSpawnerManager(World& world)
    : world(world)
{
}

void DuckSpawnerManager::Update(float deltaTime) {
    // Get the game entity that holds round state
    auto gameEntities = world.EntityManager.GetEntitiesWith<GameRoundComponent, DuckUIStateComponent>();

    if (gameEntities.empty()) {
        return;  // No game entity exists yet
    }

    Entity* gameEntity = gameEntities[0];
    auto& roundComp = gameEntity->getComponent<GameRoundComponent>();
    auto& uiState = gameEntity->getComponent<DuckUIStateComponent>();

    if (roundComp.ducksSpawned >= roundComp.maxDucksPerRound) {
        return;  // All ducks for this round have been spawned
    }

    // Update spawn timer
    spawnTimer += deltaTime;

    if (spawnTimer >= spawnInterval) {
        spawnTimer = 0.0f;
        spawnDuck();

        // Update game state - mark duck as spawned in UI
        if (roundComp.ducksSpawned < roundComp.maxDucksPerRound) {
            uiState.states[roundComp.ducksSpawned] = DuckState::SPAWNED;
            roundComp.ducksSpawned++;
        }
    }
}

void DuckSpawnerManager::spawnDuck() {
    // Get camera vectors for positioning
    glm::vec3 cameraFwd = world.camera->front;
    glm::vec3 cameraRight = world.camera->right;
    glm::vec3 cameraUp = world.camera->up;

    // Determine spawn side: 50% Left (-1), 50% Right (+1) relative to camera
    bool spawnLeft = (rand() % 2) == 0;
    float sideMultiplier = spawnLeft ? -1.0f : 1.0f;

    float distForward = 15.0f;  // Distance in front of camera
    float distSide = 35.0f;     // How far to the left/right
    float randUp = 0.0f + (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 6.0f;

    // Position = CameraPos + (Forward * depth) + (Right * side) + (Up * height)
    glm::vec3 spawnPos = world.camera->position
                       + (cameraFwd * distForward)
                       + (cameraRight * (distSide * sideMultiplier))
                       + (cameraUp * randUp);

    // Get duck speed based on current round
    auto gameEntities = world.EntityManager.GetEntitiesWith<GameRoundComponent>();
    if (gameEntities.empty()) return;

    auto& roundComp = gameEntities[0]->getComponent<GameRoundComponent>();
    float speed = roundComp.getDuckSpeed();

    // Create duck using DuckFactory
    // DuckFactory handles all component setup including random flight direction
    Entity* newDuck = DuckFactory::createDuck(world, spawnPos, speed);
}
