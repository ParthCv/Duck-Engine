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
    // Note: Entity creation moved to beginPlay() to ensure OpenGL context exists 
    // before meshes are loaded.
    
    std::cout << "Entity Length: "<< EntityManager.GetEntities().size() << std::endl;
    collisionSystem = new CollisionSystem();

    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

void World::update(float deltaTime)
{
    EntityManager.Update(deltaTime);
}

void World::beginPlay()
{
    addLightsToWorld();
    std::cout << "Directional lights: " << lightManager.getDirectionalLightCount() << std::endl;
    std::cout << "Point lights: " << lightManager.getPointLightCount() << std::endl;

    // --- MOVE INITIAL DUCK SPAWN HERE ---
    // This ensures OpenGL context is ready for mesh loading in DuckEntity constructor
    // for (size_t i = 0; i < duckPos.size(); ++i) {
    //     DuckEntity& Duck = EntityManager.CreateDuckEntity(*this, duckPos[i]);
    // }

    EntityManager.BeginPlay();

    Entity& PlayerEntity = EntityManager.CreateEntity(*this);

    glm::vec3 camPos = camera->position;

    PlayerEntity.addComponent<Transform>(camPos, glm::vec3(0.0f), glm::vec3(1.0f));

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