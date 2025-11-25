//
// Created by User on 11/18/2025.
//

#include "DuckEntity.h"
#include "../ecs/World.h"
#include "../ecs/Component.h"
#include "GLFW/glfw3.h"
#include <cstdlib>
#include <iostream>

DuckEntity::DuckEntity(World& InWorld) : Entity(InWorld)
{
    auto& transform = addComponent<Transform>(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(10,10,10));

    // Initialize Velocity
    addComponent<Velocity>(glm::vec3(1.0, 0.0f, 0.0f), 0.25f);

    // Initialize Collider
    auto& collider = addComponent<BoxCollider>();

    // Initialize Mesh
    auto& staticMeshComponent = addComponent<StaticMeshComponent>(*this);
    staticMeshComponent.loadMesh("../assets/models/duck.obj");

    // Configure Collider based on Mesh
    collider.size = staticMeshComponent.Mesh->getSize() * transform.scale;
    collider.center = staticMeshComponent.Mesh->getCenter();
    collider.center.y += collider.size.y / 2.0f;

    // Initialize Debug
    auto& debug = addComponent<DebugDrawable>();
    debug.drawCollider = true;
    debug.colliderColor = glm::vec3(1.0f, 0.0f, 0.0f); // Red
    addComponent<StaticMeshComponent>(*this);

}

DuckEntity::DuckEntity(World &InWorld, glm::vec3 &InPosition) : Entity(InWorld) {
    auto& transform = addComponent<Transform>(InPosition, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(10,10,10));

    spawnPosition = InPosition;

    addComponent<Velocity>(glm::vec3(0.0, 0.0f, 0.0f), 0.0f);
    addComponent<StaticMeshComponent>(*this );

    // Initialize Mesh
    auto& staticMeshComponent = addComponent<StaticMeshComponent>(*this);
    staticMeshComponent.loadMesh("../assets/models/duck.obj");

    // Initialize Collider
    auto& collider = addComponent<BoxCollider>();
    collider.size = staticMeshComponent.Mesh->getSize() * transform.scale;
    collider.center = staticMeshComponent.Mesh->getCenter();
    collider.center.y += collider.size.y / 2.0f;

    auto& debug = addComponent<DebugDrawable>();
    debug.drawCollider = true;
    debug.colliderColor = glm::vec3(1.0f, 0.0f, 0.0f);

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
    // Mesh loading moved to constructor
}

void DuckEntity::setRandomFlightPath() {

    auto& EntityTransform = this->getComponent<Transform>();
    auto& EntityStaticMesh = this->getComponent<StaticMeshComponent>();
    auto& EntityVelocity = this->getComponent<Velocity>();

    // TODO: Rotate to face 45 degrees up into the air
    EntityTransform.LocalRotate(-45.0f, glm::vec3(1.0f, 0.0f, 0.0f));

    bool moveRight = EntityTransform.position.x < 0.0f;
    float dirX = moveRight ? 1.0f : -1.0f;

    // Rotate to face the flight direction
    EntityTransform.rotation = glm::quat(glm::vec3(0.0f));
    float rotationAngle = moveRight ? 90.0f : -90.0f;
    EntityTransform.WorldRotate(rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));

    // TODO: Set Entity Velocity
    EntityVelocity.setVelocity(glm::vec3(dirX, 0.0f, 1.0f), 1.0f);
}

void DuckEntity::checkIfEscaped()
{
    auto& EntityTransform = this->getComponent<Transform>();
    // Increased escape distance slightly since they spawn further out
    if (glm::distance(EntityTransform.position, spawnPosition) > 100.0f)
    {
        this->destroy();
    }
}

void DuckEntity::KillDuck()
{
    this->destroy();
    std::cout << "Duck Died" << std::endl;
    // TODO: Increment GameState points here.
    // ...
}
