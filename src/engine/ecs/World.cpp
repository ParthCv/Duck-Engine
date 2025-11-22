#include "World.h"
#include <iostream>
#include <random>
#include <ctime>

#include "Component.h"
#include "../game/DuckEntity.h"
#include "../renderer/Camera.h"
#include "GLFW/glfw3.h"
#include "../system/CollisionSystem.h"

World::World()
{
    // Entity& FirstEntity = EntityManager.CreateEntity(*this);

    // TODO: Create first entity and test if we can move it per update.
    // Entity& FirstEntity = CreateEntity(*this);

    // auto& transform = FirstEntity.AddComponent<Transform>(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1, 1, 1));
    // FirstEntity.AddComponent<Velocity>(glm::vec3(0.0f, 0.0f, 0.0f));
    // FirstEntity.AddComponent<StaticMeshComponent>(FirstEntity, transform);

    // TEMP LOADINg DUCKS RN
    for (size_t i = 0; i < duckPos.size(); ++i) {
        DuckEntity& Duck = EntityManager.CreateDuckEntity(*this, duckPos[i]);
    }
    std::cout << "Entity Length: "<< EntityManager.GetEntities().size() << std::endl;
    collisionSystem = new CollisionSystem();
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

        // update game logic (ducks, score, etc.) - will add later
    }

    // TODO: Get the first entity
    // auto& FirstEntity = EntityManager.GetEntities()[0];
    // auto& FirstEntityTransform = FirstEntity->GetComponent<Transform>();
    // auto& FirstEntityStaticMesh = FirstEntity->GetComponent<StaticMeshComponent>();

    // TODO: Manually transforming the First Entity every update.
    // FirstEntityTransform.position.x += 1.0f * deltaTime;
    // std::cout << "Entity One Transform.X: " << FirstEntityTransform.position.x << std::endl;

    // TODO: Adding to the Entities' Transform
    // FirstEntityTransform.AddTransform(glm::vec3(1.0f, 0, 0) * deltaTime);

    // TODO: Rotate the StaticMesh
    // Rotating the Owning Entity, not the StaticMeshComponent
    // Entity& OwningEntity = *FirstEntityStaticMesh.OwningEntity;
    // auto* OwningEntityTransform = &OwningEntity.GetComponent<Transform>();
    // OwningEntityTransform->Rotate(glm::vec3(0.005, 0 ,0));

    TestRandomRaycasting(deltaTime);
    // TODO: Rotating the StaticMeshComponent, not the OwningEntity
    // FirstEntityStaticMesh.StaticMeshTransform.Rotate(glm::vec3(0, 10,0) * deltaTime);

    // TODO: Moving the StaticMeshComponent, not the OwningEntity
    // FirstEntityStaticMesh.StaticMeshTransform.AddTransform(glm::vec3(1.0f, 0, 0) * deltaTime);

    // TODO: Rotate entity transform
    // FirstEntityTransform.Rotate(glm::vec3(0,10,0) * deltaTime);
    // glm::mat4 model = FirstEntityTransform.Rotate(FirstEntityStaticMesh.getTransformMatrix(),180 * deltaTime,glm::vec3(0,1,0));

    // srand(time(0));
    // int randX = rand() % 2;
    // int randY = rand() % 2;
    // int randZ = rand() % 2;
    // FirstEntityStaticMesh.setPosition(glm::vec3(0,randY,0));


    // model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f)); // Rotate


    // FirstEntityStaticMesh.Transform->SetTransform(FirstEntityTransform.position + glm::vec3(0, randY, 0) * deltaTime);
    // FirstEntityStaticMesh.Transform->SetTransform(FirstEntityTransform.position);
    TestRandomRaycasting(deltaTime);
    EntityManager.Update(deltaTime);
}

void World::beginPlay()
{
    addLightsToWorld();
    std::cout << "Directional lights: " << lightManager.getDirectionalLightCount() << std::endl;
    std::cout << "Point lights: " << lightManager.getPointLightCount() << std::endl;

    // TODO: Call all system beginPlay below.
    EntityManager.BeginPlay();

    // TODO: Implement all other beginPlay logic below.
    // auto& FirstEntity = EntityManager.GetEntities()[0];
    // if (FirstEntity->HasComponent<StaticMeshComponent>())
    // {
    //     auto& FirstEntityStaticMeshComponent = FirstEntity->GetComponent<StaticMeshComponent>();
    //
    //     // FirstEntityStaticMeshComponent.Transform->SetTransform(glm::vec3(0,2,0));
    //
    //     CreateCube(FirstEntityStaticMeshComponent.VAO, FirstEntityStaticMeshComponent.VBO);
    // }
}

void World::addLightsToWorld() {
    DirectionalLight sunLight(
        glm::vec3(-0.5f, -1.0f, -0.3f),
        glm::vec3(1.0f, 0.95f, 0.9f),
        1.0f
    );
    lightManager.addDirectionalLight(sunLight);

    // Add point lights
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
        // Top face (Y+)
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    glGenVertexArrays(1, &InVAO);
    glGenBuffers(1, &InVBO);
    glGenVertexArrays(1, &inVAO);
    glGenBuffers(1, &inVBO);

    glBindVertexArray(InVAO);
    glBindBuffer(GL_ARRAY_BUFFER, InVBO);
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
