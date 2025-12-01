#pragma once
#include "../src/engine/renderer/light/LightManager.h"
#include "../src/engine/renderer/Camera.h"
#include "../ecs/system/EntityManager.h"
#include "../ecs/system/DebugRenderSystem.h"
#include "../ecs/system/CollisionSystem.h"
#include "../ecs/system/DuckSpawnerManager.h"
#include "../ecs/system/MovementSystem.h"
#include "../ecs/system/BoundsSystem.h"
#include "../ecs/system/LifecycleSystem.h"
#include "../game/EventQueue.h"
#include "../game/ecs/systems/GameRoundSystem.h"
#include "../game/ecs/systems/GunSystem.h"
#include "../game/ecs/systems/ShootingSystem.h"

class DuckSpawnerManager;

class World
{
public:
    Camera* camera;

    EntityManager EntityManager;
    LightManager lightManager;

    CollisionSystem* collisionSystem = nullptr;
    DebugRenderSystem debugRenderSystem;
    DuckSpawnerManager* duckSpawnerManager;
    MovementSystem movementSystem;
    BoundsSystem boundsSystem;
    LifecycleSystem lifecycleSystem;
    GunSystem gunSystem;

    GameRoundSystem gameRoundSystem;
    ShootingSystem shootingSystem;

    EventQueue events;

    World();

    void update(float deltaTime);
    void beginPlay();
    void addLightsToWorld();
    void cleanUp();

    Entity* getGameEntity();

    int getCurrentRound() const;
    int getScore() const;
    int getBullets() const;
    int getMaxBullets() const;
    bool hasBullets() const;
private:
    Entity* gunEntity = nullptr;
    Entity* cachedGameEntity = nullptr;

    // Recoil State
    float gunRecoilOffset = 0.0f; // Z-axis kickback
    float gunRecoilPitch = 0.0f;  // Upward barrel rotation
};
