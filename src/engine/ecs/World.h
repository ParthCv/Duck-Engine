#pragma once
#include "../src/engine/renderer/light/LightManager.h"
#include "../src/engine/renderer/Shader.h"
#include "../src/engine/renderer/Camera.h"
#include "../ecs/system/EntityManager.h"
#include "../ecs/system/DebugRenderSystem.h"
#include "../ecs/system/CollisionSystem.h"
#include "../ecs/system/DuckSpawnerManager.h"
#include "../ecs/system/MovementSystem.h"
#include "../ecs/system/BoundsSystem.h"
#include "../ecs/system/LifecycleSystem.h"
#include "../ecs/system/GunSystem.h"
#include "../game/ecs/system/DuckDeathSystem.h"

class DuckSpawnerManager;

class World
{
public:
    // TODO: Remove later: Temp Variables
    Camera* camera;
    Shader* basicShader;
    EntityManager EntityManager;
    LightManager lightManager;
    CollisionSystem* collisionSystem = nullptr;
    DebugRenderSystem debugRenderSystem;
    DuckSpawnerManager* duckSpawnerManager;
    MovementSystem movementSystem;
    BoundsSystem boundsSystem;
    LifecycleSystem lifecycleSystem;
    DuckDeathSystem duckDeathSystem;
    GunSystem gunSystem;

    World();

    void update(float deltaTime);

    void beginPlay();

    void addLightsToWorld();

    void cleanUp();
private:
    Entity* gunEntity = nullptr;

    // Recoil State
    float gunRecoilOffset = 0.0f; // Z-axis kickback
    float gunRecoilPitch = 0.0f;  // Upward barrel rotation
};
