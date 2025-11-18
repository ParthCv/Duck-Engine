//
// Created by User on 11/18/2025.
//

#include "DuckEntity.h"
#include "../ecs/Component.h"
#include "GLFW/glfw3.h"

DuckEntity::DuckEntity(World& InWorld) : Entity(InWorld)
{
    auto& transform = AddComponent<Transform>(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1, 1, 1));
    AddComponent<Velocity>(glm::vec3(0.0f, 0.0f, 0.0f));
    AddComponent<StaticMeshComponent>(*this, transform);
}

DuckEntity::~DuckEntity()
{

}

void DuckEntity::Update(float deltaTime) {
    Entity::Update(deltaTime);

    auto& EntityTransform = this->GetComponent<Transform>();
    auto& EntityStaticMesh = this->GetComponent<StaticMeshComponent>();

    EntityTransform.AddTransform(glm::vec3(0.25f, 0, 0) * deltaTime);

    accumulatedTime += deltaTime;
    float randY = std::sin(accumulatedTime);
    EntityStaticMesh.SetPosition(glm::vec3(0,randY,0));
}

void DuckEntity::BeginPlay() {
    Entity::BeginPlay();
}
