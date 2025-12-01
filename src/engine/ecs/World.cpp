#include "World.h"
#include <iostream>
#include <random>
#include <ctime>

#include "Component.h"
#include "../core/managers/GameStateManager.h"
#include "../game/DuckFactory.h"
#include "../renderer/Camera.h"
#include "GLFW/glfw3.h"
#include "../system/CollisionSystem.h"
#include "../game/EnvironmentGenerator.h"
#include "../core/managers/InputManager.h"
#include "../core/managers/ResourceManager.h"

World::World()
{
    // Note: Entity creation moved to beginPlay() to ensure OpenGL context exists
    // before meshes are loaded.

    std::cout << "Entity Length: "<< registry.getAllEntities().size() << std::endl;
    collisionSystem = new CollisionSystem();

    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Set up lights
    addLightsToWorld();
    std::cout << "Directional lights: " << lightManager.getDirectionalLightCount() << std::endl;
    std::cout << "Point lights: " << lightManager.getPointLightCount() << std::endl;

    duckSpawnerManager = new DuckSpawnerManager(*this);

    GameStateManager::get().resetDuckStates();  // Initialize Duck State array
}

void World::update(float deltaTime)
{
    float time = glfwGetTime();

    // --- RECOIL LOGIC ---
    // Recovery from recoil (Lerp back to 0), Higher recoverySpeed = snappier recovery
    float recoverySpeed = 10.0f;
    gunRecoilOffset = glm::mix(gunRecoilOffset, 0.0f, deltaTime * recoverySpeed);
    gunRecoilPitch  = glm::mix(gunRecoilPitch, 0.0f, deltaTime * recoverySpeed);

    // if (lightManager.getDirectionalLightCount() > 0) {
    //     auto& dirLight = lightManager.getDirectionalLight(0);
    //
    //     // Rotate around Y axis
    //     float angle = deltaTime * 0.2f; // Rotation speed
    //     glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));
    //     dirLight.direction = glm::vec3(rotation * glm::vec4(dirLight.direction, 0.0f));
    // }

    movementSystem.update(*this, deltaTime);
    boundsSystem.update(*this, deltaTime);
    lifecycleSystem.update(*this, deltaTime);
    gunSystem.update(*this, *camera, deltaTime);

    //EntityManager.Update(deltaTime);
    duckSpawnerManager->Update(deltaTime);
}

void World::beginPlay()
{
    EntityID duck = registry.createEntity();
    EntityID player = registry.createEntity();

    registry.addComponent(duck, Transform{{0, 0, 0}});
    registry.addComponent(duck, Velocity{{1, 0, 0}, 5.0f});
    registry.addComponent(player, Transform{{10, 0, 0}});

    glm::vec3 camPos = camera->position;

    // Create a player entity to act as the source for raycasting
    EntityID playerEid = registry.createEntity();
    registry.addComponent(playerEid, Transform{camPos, glm::vec3(0.0f), glm::vec3(1.0f)});

    // Create the gun entity (GunSystem will handle positioning and rotation)
    EntityID gunEid = registry.createEntity();
    Transform gunTransform;
    gunTransform.scale = glm::vec3{1.f, 1.f, 1.f};
    registry.addComponent(gunEid, gunTransform);
    StaticMeshComponent gunStaticMeshComponent;
    gunStaticMeshComponent.Mesh = ResourceManager::Get().GetStaticMesh("rifle.obj");
    registry.addComponent(gunEid, gunStaticMeshComponent);

    EnvironmentGenerator envGenerator{*this, EntityManager};
    envGenerator.generate(20.f, 64, 5, 20.f, glm::vec3(0.f));

    // Add raycast source component
    RaycastSource raySource;
    raySource.maxDistance = 100.0f;
    raySource.drawRay = true;
    raySource.direction = glm::vec3(0.0f, 0.0f, -1.0f);
    registry.addComponent(playerEid, raySource);

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
