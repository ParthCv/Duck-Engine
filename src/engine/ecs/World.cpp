#include "World.h"
#include <iostream>
#include <random>
#include <ctime>

#include "Component.h"
#include "../game/DuckFactory.h"
#include "../game/GunEntity.h"
#include "../renderer/Camera.h"
#include "GLFW/glfw3.h"
#include "../system/CollisionSystem.h"
#include "../game/EnvironmentGenerator.h"
#include "../core/managers/InputManager.h"

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
}

void World::update(float deltaTime)
{
    float time = glfwGetTime();

    // --- RECOIL LOGIC ---
    // Recovery from recoil (Lerp back to 0), Higher recoverySpeed = snappier recovery
    float recoverySpeed = 10.0f;
    gunRecoilOffset = glm::mix(gunRecoilOffset, 0.0f, deltaTime * recoverySpeed);
    gunRecoilPitch  = glm::mix(gunRecoilPitch, 0.0f, deltaTime * recoverySpeed);

    // Recoil on input (Update numbers to liking)
    if (InputManager::isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
        gunRecoilOffset = 0.2f;  // Move back
        gunRecoilPitch = 0.1f;   // Rotate up
    }

    // --- FPS GUN UPDATE LOGIC ---
    if (gunEntity && camera) {
        auto& transform = gunEntity->getComponent<Transform>();

        // 1. POSITION: Lock to camera with offset + Recoil Kickback
        // Recoil moves the gun opposite to the camera front vector
        glm::vec3 kickback = -camera->front * gunRecoilOffset;

        // Base Offset: Right: +0.25f, Down: -0.25f, Forward: +0.5f
        glm::vec3 offset = (camera->right * 0.25f) + (camera->up * -0.25f) + (camera->front * 0.5f);

        transform.position = camera->position + offset + kickback;

        // 2. ROTATION: Lock to camera orientation + Recoil Pitch
        // Construct a rotation matrix from the camera's basis vectors
        glm::mat3 camRotation(camera->right, camera->up, -camera->front);
        glm::quat orientation = glm::quat_cast(camRotation);

        // Apply muzzle climb (rotate around camera right axis)
        glm::quat recoilRot = glm::angleAxis(gunRecoilPitch, camera->right);
        orientation = recoilRot * orientation;
        orientation = orientation * glm::angleAxis(glm::radians(180.0f), glm::vec3(0, 1, 0));

        transform.rotation = orientation;
    }

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
    EntityManager.CreateEntityOfType<GunEntity>(*this, "rifle.obj");

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
