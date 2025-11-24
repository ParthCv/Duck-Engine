//
// Created by User on 11/18/2025.
//

#include "DuckEntity.h"
#include "../ecs/World.h"
#include "../ecs/Component.h"
#include "GLFW/glfw3.h"

DuckEntity::DuckEntity(World& InWorld) : Entity(InWorld)
{
    auto& transform = addComponent<Transform>(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1,1,1));

    float randX = rand() % 2;
    float randZ = rand() % 2;

    addComponent<Velocity>(glm::vec3(1.0, 0.0f, 0.0f), 0.25f);
    addComponent<StaticMeshComponent>(*this);
}

DuckEntity::DuckEntity(World &InWorld, glm::vec3 &InPosition) : Entity(InWorld) {
    auto& transform = addComponent<Transform>(InPosition, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1,1,1));

    spawnPosition = InPosition;

    addComponent<Velocity>(glm::vec3(0.0, 0.0f, 0.0f), 0.0f);
    addComponent<StaticMeshComponent>(*this);
}

DuckEntity::~DuckEntity() {}

void DuckEntity::update(float deltaTime) {
    Entity::update(deltaTime);

    // TODO: Get Components
    auto& EntityTransform = this->getComponent<Transform>();
    auto& EntityVelocity = this->getComponent<Velocity>();
    auto& EntityStaticMesh = this->getComponent<StaticMeshComponent>();

    // TODO: Move Duck by Velocity.
    EntityTransform.AddLocalTransform(EntityVelocity.Direction * EntityVelocity.Speed * deltaTime);

    // TODO: Manually move StaticMeshComponent in a Sin wave manner.
    // accumulatedTime += deltaTime;
    // float randY = std::sin(accumulatedTime);
    // EntityStaticMesh.setPosition(glm::vec3(0,randY,0));

    // Entity& OwningEntity = *EntityStaticMesh.OwningEntity;
    // auto* OwningEntityTransform = &OwningEntity.getComponent<Transform>();
    // OwningEntityTransform->Rotate(glm::vec3(0.005, 0.001 ,0));

    checkIfEscaped();
}

void DuckEntity::beginPlay() {
    Entity::beginPlay();

    // TODO: Implement specific beginPlay logic for only DuckEntity here.
    auto& EntityStaticMesh = this->getComponent<StaticMeshComponent>();
    world->CreateCube(EntityStaticMesh.VAO, EntityStaticMesh.VBO);

    // TODO: Set the flight path
    setRandomFlightPath();
}

void DuckEntity::setRandomFlightPath() {

    auto& EntityTransform = this->getComponent<Transform>();
    auto& EntityStaticMesh = this->getComponent<StaticMeshComponent>();
    auto& EntityVelocity = this->getComponent<Velocity>();

    // TODO: Rotate to face 45 degrees up into the air
    EntityTransform.LocalRotate(-45.0f, glm::vec3(1.0f, 0.0f, 0.0f));

    // TODO: Rotate around Y-Axis randomly
    float randomAngle = rand() % 360;
    EntityTransform.WorldRotate(randomAngle, glm::vec3(0.0f, 1.0f, 0.0f));

    // TODO: Set Entity Velocity
    EntityVelocity.setVelocity(glm::vec3(0.0f, 0.0f, 1.0f), 1.0f);
}

void DuckEntity::checkIfEscaped()
{
    auto& EntityTransform = this->getComponent<Transform>();
    if (glm::distance(EntityTransform.position, spawnPosition) > escapeDistance)
    {
        this->destroy();
    }
}

void DuckEntity::KillDuck()
{
    this->destroy();

    // TODO: Increment GameState points here.
    // ...
}
