#include "World.h"
#include <iostream>
#include <random>
#include <ctime>
#include <cstdlib>

#include "Component.h"
#include "../game/DuckEntity.h"
#include "../renderer/Camera.h"
#include "GLFW/glfw3.h"
#include "../system/CollisionSystem.h"

World::World()
{
    // TEMP LOADINg DUCKS RN
    for (size_t i = 0; i < duckPos.size(); ++i) {
        DuckEntity& Duck = EntityManager.CreateDuckEntity(*this, duckPos[i]);
    }
    std::cout << "Entity Length: "<< EntityManager.GetEntities().size() << std::endl;
    collisionSystem = new CollisionSystem();

    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

void World::update(float deltaTime)
{
    float time = glfwGetTime();

    float radius = 10.0f;
    float height = 3.0f;

    camera->position.x = sin(time * 0.5f) * radius;
    camera->position.z = cos(time * 0.5f) * radius;
    camera->position.y = height;

    camera->target = glm::vec3(0.0f, 2.0f, 0.0f);

    if (lightManager.getPointLightCount() > 1) {
        auto& light = lightManager.getPointLight(1);
        light.position.x = sin(time) * 3.0f;
        light.position.z = cos(time) * 3.0f;
    }

    testRandomRaycasting(deltaTime);
    EntityManager.Update(deltaTime);
}

void World::beginPlay()
{
    addLightsToWorld();
    std::cout << "Directional lights: " << lightManager.getDirectionalLightCount() << std::endl;
    std::cout << "Point lights: " << lightManager.getPointLightCount() << std::endl;

    EntityManager.BeginPlay();

    // Create a player entity to act as the source for raycasting
    Entity& PlayerEntity = EntityManager.CreateEntity(*this);

    // glm::vec3 camPos = glm::vec3(0.0f, 5.0f, 10.0f);

    glm::vec3 camPos = camera->position;

    PlayerEntity.addComponent<Transform>(camPos, glm::vec3(0.0f), glm::vec3(1.0f));

    // Add raycast source component
    auto& raySource = PlayerEntity.addComponent<RaycastSource>();
    raySource.maxDistance = 100.0f;
    raySource.drawRay = true;
    raySource.direction = glm::vec3(0.0f, 0.0f, -1.0f);
}

void World::addLightsToWorld() {
    DirectionalLight sunLight(
        glm::vec3(-0.5f, -1.0f, -0.3f),
        glm::vec3(1.0f, 0.95f, 0.9f),
        1.0f
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

void World::testRandomRaycasting(float deltaTime)
{
    static float timeUntilNextFire = 0.5f;
    timeUntilNextFire -= deltaTime;

    auto raySourceEntities = EntityManager.GetEntitiesWith<RaycastSource>();
    if (raySourceEntities.empty()) return;

    auto* rayEntity = raySourceEntities[0];
    auto& raySource = rayEntity->getComponent<RaycastSource>();

    if (!rayEntity->hasComponent<Transform>()) return;
    auto& transform = rayEntity->getComponent<Transform>();

    if (camera) {
        glm::vec3 cameraForward = glm::normalize(camera->target - camera->position);
        glm::vec3 cameraRight = glm::normalize(glm::cross(cameraForward, glm::vec3(0, 1, 0)));
        glm::vec3 cameraUp = glm::normalize(glm::cross(cameraRight, cameraForward));

        transform.position = camera->position + (cameraForward * 1.0f) + (cameraUp * 1.0f);
    }

    if (timeUntilNextFire <= 0.0f) {
        timeUntilNextFire = 0.1f + static_cast<float>(rand()) / (RAND_MAX / 0.4f);

        auto targets = EntityManager.GetEntitiesWith<BoxCollider, Transform>();

        if (!targets.empty()) {
            int randomIndex = rand() % targets.size();
            Entity* targetEntity = targets[randomIndex];

            glm::vec3 targetPos = targetEntity->getComponent<Transform>().position;

            float noiseAmount = 2.0f;
            targetPos.x += ((float)rand() / RAND_MAX - 0.5f) * noiseAmount;
            targetPos.y += ((float)rand() / RAND_MAX - 0.5f) * noiseAmount;
            targetPos.z += ((float)rand() / RAND_MAX - 0.5f) * noiseAmount;
            raySource.direction = glm::normalize(targetPos - transform.position);

            if (collisionSystem) {
                collisionSystem->RaycastFromEntity(EntityManager, *rayEntity);
            }
        }
    }
}


void World::CreateCube(GLuint& inVAO, GLuint& inVBO) {
    float vertices[] = {
        // Positions          // Normals           // TexCoords
        // Front face (Z+)
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        // Back face (Z-)
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
        // Left face (X-)
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        // Right face (X+)
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        // Bottom face (Y-)
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
        // Top face (Y+)
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    glGenVertexArrays(1, &inVAO);
    glGenBuffers(1, &inVBO);

    glBindVertexArray(inVAO);
    glBindBuffer(GL_ARRAY_BUFFER, inVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // TexCoord (location = 2)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}