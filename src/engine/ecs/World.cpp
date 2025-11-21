//
// Created by super on 2025-11-03.
//

#include "World.h"
#include <iostream>
#include <random>
#include <ctime>

#include "Component.h"
#include "../renderer/Camera.h"
#include "../renderer/Shader.h"
#include "GLFW/glfw3.h"
#include "../debug/DebugRenderer.h"
#include "../system/CollisionSystem.h"
#include "../system/DebugRenderSystem.h"

World::World()
{
    collisionSystem = new CollisionSystem();
}

void World::Update(float deltaTime)
{
    // FIXED: No more EntityManager.GetEntities()[0]!
    // Find any entity that has both a StaticMeshComponent and a BoxCollider (this is our target cube)
    auto cubeEntities = EntityManager.GetEntitiesWith<StaticMeshComponent, BoxCollider>();

    if (!cubeEntities.empty())
    {
        // Just take the first one that matches — safe and scalable
        auto* cubeEntity = cubeEntities[0];
        auto& transform = cubeEntity->GetComponent<Transform>();
        auto& staticMesh = cubeEntity->GetComponent<StaticMeshComponent>();

        // Move the entity (affects both visuals and collider)
        transform.AddTransform(glm::vec3(1.0f, 0, 0) * deltaTime);

        // Rotate the mesh component and transform
        staticMesh.StaticMeshTransform.Rotate(glm::vec3(0, 10, 0) * deltaTime);
        staticMesh.StaticMeshTransform.AddTransform(glm::vec3(1.0f, 0, 0) * deltaTime);
    }

    TestRandomRaycasting(deltaTime);
    EntityManager.Update();
}

void World::Render()
{
    // Clear with color
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use shader
    basicShader->use();

    glm::mat4 view = camera->getViewMatrix();
    glm::mat4 projection = camera->getProjectionMatrix();

    basicShader->setMat4("view", view);
    basicShader->setMat4("projection", projection);

    // FIXED: No more [0] — properly iterate over all entities with StaticMeshComponent
    auto renderableEntities = EntityManager.GetEntitiesWith<StaticMeshComponent>();

    for (auto* entity : renderableEntities)
    {
        if (!entity->GetIsActive()) continue;

        auto& staticMeshComp = entity->GetComponent<StaticMeshComponent>();

        // Use the component's transform matrix (which includes local rotation)
        glm::mat4 model = staticMeshComp.GetTransformMatrix();

        basicShader->setMat4("model", model);

        glBindVertexArray(staticMeshComp.VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    debugRenderSystem.Render(EntityManager, *basicShader);
    glBindVertexArray(0);
}

void World::BeginPlay()
{
    Entity& FirstEntity = EntityManager.CreateEntity(*this);

    // Add required components
    FirstEntity.AddComponent<Transform>(
     glm::vec3(0.0f, 0.0f, 0.0f),  // position
     glm::vec3(0.0f, 0.0f, 0.0f),  // rotation
     glm::vec3(1.0f, 1.0f, 1.0f)   // scale
 );
    auto& meshComp = FirstEntity.AddComponent<StaticMeshComponent>(FirstEntity);
    FirstEntity.AddComponent<BoxCollider>(); // This is critical for TestGame to hit!
    FirstEntity.AddComponent<DebugDrawable>();

    // FIXED: Safe query instead of assuming [0]
    auto entities = EntityManager.GetEntitiesWith<StaticMeshComponent>();
    if (!entities.empty())
    {
        auto* entity = entities[0];
        auto& meshComp = entity->GetComponent<StaticMeshComponent>();
        CreateCube(meshComp.VAO, meshComp.VBO);
    }

    // Create a debug ray entity
    Entity& rayEntity = EntityManager.CreateEntity(*this);
    rayEntity.AddComponent<Transform>(
     glm::vec3(0.0f, -1.0f, 1.0f),
     glm::vec3(0.0f, 0.0f, 0.0f),
     glm::vec3(0.0f, 0.0f, 0.0f)
 );
    auto& raySource = rayEntity.AddComponent<RaycastSource>();

    // Enable drawing
    raySource.drawRay = true;
    raySource.maxDistance = 100.0f;

    // Sets up debug renderer
    DebugRenderer::getInstance().init();
}

void World::CleanUp()
{
    DebugRenderer::getInstance().cleanup();
}

void World::TestRandomRaycasting(float deltaTime)
{
    static float timeSinceLastRay = 0.0f;
    const float rayInterval = 1.0f; // 1 second

    timeSinceLastRay += deltaTime;

    // Only run once per second
    if (timeSinceLastRay >= rayInterval)
    {
        timeSinceLastRay = 0.0f; // Reset timer

        auto raySourceEntities = EntityManager.GetEntitiesWith<RaycastSource>();
        if (raySourceEntities.empty()) return;

        auto* rayEntity = raySourceEntities[0];
        auto& raySource = rayEntity->GetComponent<RaycastSource>();

        static std::mt19937 generator(static_cast<unsigned int>(time(0)));
        static std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);

        glm::vec3 randomDirection = glm::vec3(
            distribution(generator),
            distribution(generator),
            distribution(generator)
        );

        if (glm::length(randomDirection) > 0.0f) {
            raySource.direction = glm::normalize(randomDirection);
        } else {
            raySource.direction = glm::vec3(0.0f, 0.0f, 1.0f);
        }
        collisionSystem->RaycastFromEntity(EntityManager, *rayEntity);

        // Debug output
        std::cout << "Ray Fired! Hit: " << (raySource.lastHit ? "YES" : "NO") << std::endl;
    }
}

void World::CreateCube(GLuint& InVAO, GLuint& InVBO)
{
    // Cube vertices with colors
    float vertices[] = {
        // Positions          // Colors
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f
    };

    glGenVertexArrays(1, &InVAO);
    glGenBuffers(1, &InVBO);

    glBindVertexArray(InVAO);
    glBindBuffer(GL_ARRAY_BUFFER, InVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void World::CreateLine(GLuint& InVAO, GLuint& InVBO) {
    glGenVertexArrays(1, &InVAO);
    glGenBuffers(1, &InVBO);

    glBindVertexArray(InVAO);
    glBindBuffer(GL_ARRAY_BUFFER, InVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}