//
// Created by super on 2025-11-03.
//

#include "World.h"
#include <iostream>
#include "Component.h"
#include "../renderer/Camera.h"
#include "../renderer/Shader.h"
#include "GLFW/glfw3.h"

World::World()
{
    Entity& FirstEntity = EntityManager.CreateEntity(*this);

    // TODO: Create first entity and test if we can move it per Update.
    // Entity& FirstEntity = CreateEntity(*this);

    auto& transform = FirstEntity.AddComponent<Transform>(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1, 1, 1));
    FirstEntity.AddComponent<Velocity>(glm::vec3(0.0f, 0.0f, 0.0f));
    FirstEntity.AddComponent<StaticMeshComponent>(FirstEntity, transform);
}

void World::Update(float deltaTime)
{

    // TODO: Get the first entity
    auto& FirstEntity = EntityManager.GetEntities()[0];
    auto& FirstEntityTransform = FirstEntity->GetComponent<Transform>();
    auto& FirstEntityStaticMesh = FirstEntity->GetComponent<StaticMeshComponent>();

    // TODO: Manually transforming the First Entity every Update.
    // FirstEntityTransform.position.x += 1.0f * deltaTime;
    // std::cout << "Entity One Transform.X: " << FirstEntityTransform.position.x << std::endl;

    // TODO: Adding to the Entities' Transform
    FirstEntityTransform.AddTransform(glm::vec3(1.0f, 0, 0) * deltaTime);

    // TODO: Rotate the StaticMesh
    // Rotating the Owning Entity, not the StaticMeshComponent
    Entity& OwningEntity = *FirstEntityStaticMesh.OwningEntity;
    auto* OwningEntityTransform = &OwningEntity.GetComponent<Transform>();
    // OwningEntityTransform->Rotate(glm::vec3(0.005, 0 ,0));

    // TODO: Rotating the StaticMeshComponent, not the OwningEntity
    FirstEntityStaticMesh.StaticMeshTransform.Rotate(glm::vec3(0, 10,0) * deltaTime);

    // TODO: Moving the StaticMeshComponent, not the OwningEntity
    // FirstEntityStaticMesh.StaticMeshTransform.AddTransform(glm::vec3(1.0f, 0, 0) * deltaTime);

    // TODO: Rotate entity transform
    // FirstEntityTransform.Rotate(glm::vec3(0,10,0) * deltaTime);
    // glm::mat4 model = FirstEntityTransform.Rotate(FirstEntityStaticMesh.GetTransformMatrix(),180 * deltaTime,glm::vec3(0,1,0));

    // srand(time(0));
    int randX = rand() % 2;
    int randY = rand() % 2;
    int randZ = rand() % 2;
    FirstEntityStaticMesh.SetPosition(glm::vec3(0,randY,0));


    // model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f)); // Rotate


    // FirstEntityStaticMesh.Transform->SetTransform(FirstEntityTransform.position + glm::vec3(0, randY, 0) * deltaTime);
    // FirstEntityStaticMesh.Transform->SetTransform(FirstEntityTransform.position);

    EntityManager.Update();
}

void World::Render()
{
    // TODO: Render stuff here

    // Use shader
    basicShader->use();

    glm::mat4 view = camera->getViewMatrix();
    glm::mat4 projection = camera->getProjectionMatrix();

    basicShader->setMat4("view", view);
    basicShader->setMat4("projection", projection);

    // Draw each entity
    for (auto& entity : EntityManager.GetEntities())
    {
        if (entity->HasComponent<StaticMeshComponent>())
        {
            auto& FirstEntityStaticMeshComponent = entity->GetComponent<StaticMeshComponent>();

            // Getting the Model.
            glm::mat4 model = FirstEntityStaticMeshComponent.GetTransformMatrix();

            basicShader->setMat4("model", model);

            glBindVertexArray(FirstEntityStaticMeshComponent.VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }

    glBindVertexArray(0);
}

void World::BeginPlay()
{
    // TODO: Call all system BeginPlay below.
    EntityManager.BeginPlay();

    // TODO: Implement all other BeginPlay logic below.
    auto& FirstEntity = EntityManager.GetEntities()[0];
    if (FirstEntity->HasComponent<StaticMeshComponent>())
    {
        auto& FirstEntityStaticMeshComponent = FirstEntity->GetComponent<StaticMeshComponent>();

        // FirstEntityStaticMeshComponent.Transform->SetTransform(glm::vec3(0,2,0));

        CreateCube(FirstEntityStaticMeshComponent.VAO, FirstEntityStaticMeshComponent.VBO);
    }
}

void World::CleanUp()
{
}

void World::CreateCube(GLuint& InVAO, GLuint& InVBO)
{
    // Cube vertices with colors
    float vertices[] = {
        // Positions          // Colors
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f, // Front face (red)
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f, // Back face (green)
         0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f, // Left face (blue)
        -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f, // Right face (yellow)
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f, // Bottom face (magenta)
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f, // Top face (cyan)
         0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f
    };

    // Generate VAO and VBO
    glGenVertexArrays(1, &InVAO);
    glGenBuffers(1, &InVBO);

    // Bind and upload data
    glBindVertexArray(InVAO);
    glBindBuffer(GL_ARRAY_BUFFER, InVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}
