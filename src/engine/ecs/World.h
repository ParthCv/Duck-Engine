#pragma once
#include "../src/engine/system/EntityManager.h"
#include "../src/engine/renderer/light/LightManager.h"
#include "../src/engine/renderer/Shader.h"
#include "../src/engine/renderer/Camera.h"
#include "../system/EntityManager.h"
#include "../system/DebugRenderSystem.h"
#include "../system/CollisionSystem.h"
#include "../system/DuckSpawnerManager.h"
#include "../system/MovementSystem.h"
#include "../system/BoundsSystem.h"
#include "../system/LifecycleSystem.h"
#include "../system/GunSystem.h"

class DuckSpawnerManager;

#include "glad/glad.h"

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
