//
// Created by User on 11/18/2025.
//

#include "DuckEntity.h"
#include "../ecs/World.h"
#include "../ecs/Component.h"
#include "GLFW/glfw3.h"

DuckEntity::DuckEntity(World& InWorld) : Entity(InWorld)
{
    auto& transform = AddComponent<Transform>(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f));

    float randX = rand() % 2;
    float randZ = rand() % 2;

    AddComponent<Velocity>(glm::vec3(1.0, 0.0f, 0.0f), 0.25f);
    AddComponent<StaticMeshComponent>(*this, transform);
}

DuckEntity::DuckEntity(World &InWorld, glm::vec3 &InPosition) : Entity(InWorld) {
    auto& transform = AddComponent<Transform>(InPosition, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f));

    AddComponent<Velocity>(glm::vec3(0.0, 0.0f, 0.0f), 0.0f);
    AddComponent<StaticMeshComponent>(*this, transform);
}

DuckEntity::~DuckEntity() {}

void DuckEntity::Update(float deltaTime) {
    Entity::Update(deltaTime);

    // TODO: Get Components
    auto& EntityTransform = this->GetComponent<Transform>();
    auto& EntityVelocity = this->GetComponent<Velocity>();
    auto& EntityStaticMesh = this->GetComponent<StaticMeshComponent>();

    // TODO: Move Duck by Velocity.
    EntityTransform.AddTransform(EntityVelocity.Direction * EntityVelocity.Speed * deltaTime);

    // TODO: Manually move StaticMeshComponent in a Sin wave manner.
    accumulatedTime += deltaTime;
    float randY = std::sin(accumulatedTime);
    EntityStaticMesh.SetPosition(glm::vec3(0,randY,0));

    Entity& OwningEntity = *EntityStaticMesh.OwningEntity;
    auto* OwningEntityTransform = &OwningEntity.GetComponent<Transform>();
    OwningEntityTransform->Rotate(glm::vec3(0.005, 0.001 ,0));

}

void DuckEntity::BeginPlay() {
    Entity::BeginPlay();

    // TODO: Implement specific BeginPlay logic for only DuckEntity here.
    auto& EntityStaticMesh = this->GetComponent<StaticMeshComponent>();
    world->CreateCube(EntityStaticMesh.VAO, EntityStaticMesh.VBO);
}
