#include "World.h"
#include <iostream>
#include <random>
#include <ctime>

#include "Component.h"
#include "../game/DuckFactory.h"
#include "../game/ecs/GunEntity.h"
#include "../game/ecs/components/GameRoundComponent.h"
#include "../game/ecs/system/GameStateSystem.h"
#include "../renderer/Camera.h"
#include "GLFW/glfw3.h"
#include "../ecs/system/CollisionSystem.h"
#include "../game/EnvironmentGenerator.h"

World::World()
{
    // Note: Entity creation moved to beginPlay() to ensure OpenGL context exists
    // before meshes are loaded.

    std::cout << "Entity Length: "<< EntityManager.GetEntities().size() << std::endl;
    collisionSystem = new CollisionSystem();

    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Set up lights
    addLightsToWorld();
    std::cout << "Directional lights: " << lightManager.getDirectionalLightCount() << std::endl;
    std::cout << "Point lights: " << lightManager.getPointLightCount() << std::endl;

    duckSpawnerManager = new DuckSpawnerManager(*this);

    // Duck states will be initialized when gameStateEntity is created in beginPlay()
}

void World::update(float deltaTime)
{
    float time = glfwGetTime();

    // --- RECOIL LOGIC ---
    // Recovery from recoil (Lerp back to 0), Higher recoverySpeed = snappier recovery
    float recoverySpeed = 10.0f;
    gunRecoilOffset = glm::mix(gunRecoilOffset, 0.0f, deltaTime * recoverySpeed);
    gunRecoilPitch  = glm::mix(gunRecoilPitch, 0.0f, deltaTime * recoverySpeed);

    movementSystem.update(*this, deltaTime);
    boundsSystem.update(*this, deltaTime);
    lifecycleSystem.update(*this, deltaTime);
    duckDeathSystem.update(*this, deltaTime);  // Handle duck-specific death visuals
    gunSystem.update(*this, *camera, deltaTime);

    EntityManager.Update(deltaTime);
    duckSpawnerManager->Update(deltaTime);
}

void World::beginPlay()
{
    // Create a player entity to act as the source for raycasting
    Entity& PlayerEntity = EntityManager.CreateEntity(*this);

    glm::vec3 camPos = camera->position;

    PlayerEntity.addComponent<Transform>(camPos, glm::vec3(0.0f), glm::vec3(1.0f));

    // Create the gun entity (GunSystem will handle positioning and rotation)
    gunEntity = &EntityManager.CreateEntityOfType<GunEntity>(*this, "rifle.obj");

    // This gameStateEntity stores the game state in components - following ECS principles now
    gameStateEntity = &EntityManager.CreateEntity(*this);
    gameStateEntity->addComponent<GameRoundComponent>();
    gameStateEntity->addComponent<AmmoComponent>();
    gameStateEntity->addComponent<ScoreComponent>();
    gameStateEntity->addComponent<DuckUIStateComponent>();

    // Initialize duck UI states
    auto& duckUI = gameStateEntity->getComponent<DuckUIStateComponent>();
    for (int i = 0; i < DuckUIStateComponent::MAX_DUCKS; i++) {
        duckUI.states[i] = DuckState::NOT_SPAWNED;
    }

    EnvironmentGenerator envGenerator{*this, EntityManager};
    envGenerator.generate(20.f, 64, 5, 20.f, glm::vec3(0.f));

    // Add raycast source component
    auto& raySource = PlayerEntity.addComponent<RaycastSource>();
    raySource.maxDistance = 100.0f;
    raySource.drawRay = true;
    raySource.direction = glm::vec3(0.0f, 0.0f, -1.0f);

    EntityManager.BeginPlay();
}

void World::addLightsToWorld() {
    DirectionalLight sunLight(
        glm::vec3(-0.5f, -1.0f, -0.3f),
        glm::vec3(1.0f, 0.95f, 0.9f),
        2.0f
    );
    lightManager.addDirectionalLight(sunLight);

    PointLight light1(
        glm::vec3(3.0f, 2.0f, 3.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),     // White
        10.0f,
        15.0f
    );
    lightManager.addPointLight(light1);

    PointLight light2(
        glm::vec3(-3.0f, 2.0f, -3.0f),
        glm::vec3(0.2f, 0.5f, 1.0f),     // Blue
        10.0f,
        15.0f
    );
    lightManager.addPointLight(light2);

    PointLight light3(
        glm::vec3(0.0f, 3.0f, 0.0f),
        glm::vec3(1.0f, 0.3f, 0.1f),     // Orange
        8.0f,
        12.0f
    );
    lightManager.addPointLight(light3);

    PointLight light4(
        glm::vec3(-3.0f, 1.0f, 3.0f),
        glm::vec3(0.1f, 1.0f, 0.3f),     // Green
        8.0f,
        12.0f
    );
    lightManager.addPointLight(light4);

    PointLight light5(
        glm::vec3(3.0f, 1.0f, -3.0f),
        glm::vec3(1.0f, 0.1f, 0.8f),     // Purple
        8.0f,
        12.0f
    );
    lightManager.addPointLight(light5);
}

void World::cleanUp()
{
}
