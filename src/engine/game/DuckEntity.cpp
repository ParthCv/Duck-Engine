//
// Created by User on 11/18/2025.
//

#include "DuckEntity.h"
#include "../ecs/World.h"
#include "../ecs/Component.h"
#include "GLFW/glfw3.h"

DuckEntity::DuckEntity(World& InWorld) : Entity(InWorld)
{
    auto& transform = addComponent<Transform>(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f));

    float randX = rand() % 2;
    float randZ = rand() % 2;

    addComponent<Velocity>(glm::vec3(1.0, 0.0f, 0.0f), 0.25f);
    addComponent<StaticMeshComponent>(*this, transform);
}

DuckEntity::DuckEntity(World &InWorld, glm::vec3 &InPosition) : Entity(InWorld) {
    auto& transform = addComponent<Transform>(InPosition, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f));

    addComponent<Velocity>(glm::vec3(0.0, 0.0f, 0.0f), 0.0f);
    addComponent<StaticMeshComponent>(*this, transform);
}

DuckEntity::~DuckEntity() {}

void DuckEntity::update(float deltaTime) {
    Entity::update(deltaTime);

    // TODO: Get Components
    auto& EntityTransform = this->getComponent<Transform>();
    auto& EntityVelocity = this->getComponent<Velocity>();
    auto& EntityStaticMesh = this->getComponent<StaticMeshComponent>();

    // TODO: Move Duck by Velocity.
    EntityTransform.AddTransform(EntityVelocity.Direction * EntityVelocity.Speed * deltaTime);

    // TODO: Manually move StaticMeshComponent in a Sin wave manner.
    // accumulatedTime += deltaTime;
    // float randY = std::sin(accumulatedTime);
    // EntityStaticMesh.setPosition(glm::vec3(0,randY,0));

    // Entity& OwningEntity = *EntityStaticMesh.OwningEntity;
    // auto* OwningEntityTransform = &OwningEntity.getComponent<Transform>();
    // OwningEntityTransform->Rotate(glm::vec3(0.005, 0.001 ,0));

}

void DuckEntity::beginPlay() {
    Entity::beginPlay();

    // TODO: Implement specific beginPlay logic for only DuckEntity here.
    auto& EntityStaticMesh = this->getComponent<StaticMeshComponent>();
    world->CreateCube(EntityStaticMesh.VAO, EntityStaticMesh.VBO);

    // TODO: Rotating using radians, about a specified axis.
    Entity& OwningEntity = *EntityStaticMesh.OwningEntity;
    auto* OwningEntityTransform = &OwningEntity.getComponent<Transform>();
    OwningEntityTransform->Rotate(45.0f, glm::vec3(0,1,0));
}
