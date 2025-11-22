//
// Created by User on 11/18/2025.
//

#include "DuckEntity.h"
#include "../ecs/World.h"
#include "../ecs/Component.h"
#include "GLFW/glfw3.h"

DuckEntity::DuckEntity(World& InWorld) : Entity(InWorld)
{
    // Initialize Transform
    auto& transform = addComponent<Transform>(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f));

    // Initialize Velocity
    addComponent<Velocity>(glm::vec3(1.0, 0.0f, 0.0f), 0.25f);

    // Initialize Mesh
    addComponent<StaticMeshComponent>(*this, transform);

    // Initialize Collider
    auto& collider = addComponent<BoxCollider>();
    collider.size = glm::vec3(1.1f);
    collider.center = glm::vec3(0.0f);

    // Initialize Debug
    auto& debug = addComponent<DebugDrawable>();
    debug.drawCollider = true;
    debug.colliderColor = glm::vec3(1.0f, 0.0f, 0.0f); // Red
}

DuckEntity::DuckEntity(World &InWorld, glm::vec3 &InPosition) : Entity(InWorld) {
    auto& transform = addComponent<Transform>(InPosition, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f));

    addComponent<Velocity>(glm::vec3(0.0, 0.0f, 0.0f), 0.0f);
    addComponent<StaticMeshComponent>(*this, transform);

    auto& collider = addComponent<BoxCollider>();
    collider.size = glm::vec3(1.1f);
    collider.center = glm::vec3(0.0f);

    auto& debug = addComponent<DebugDrawable>();
    debug.drawCollider = true;
    debug.colliderColor = glm::vec3(1.0f, 0.0f, 0.0f);
}

DuckEntity::~DuckEntity() {}

void DuckEntity::update(float deltaTime) {
    Entity::update(deltaTime);

    auto& EntityTransform = this->getComponent<Transform>();
    auto& EntityVelocity = this->getComponent<Velocity>();

    // 1. Move Duck Forward (Linear Movement)
    EntityTransform.position += EntityVelocity.Direction * EntityVelocity.Speed * deltaTime;

    // 2. Bob Duck Up/Down (Sine Wave)
    // FIX: Apply this to EntityTransform.position, NOT the StaticMeshComponent
    accumulatedTime += deltaTime;
    float sineOffset = std::sin(accumulatedTime) * 0.5f; // Multiplied by 0.5 to damp the bobbing height

    // We apply the sine wave to the Y axis.
    EntityTransform.position.y = sineOffset;

    // 3. Rotate the Duck
    EntityTransform.Rotate(glm::vec3(0.0f, 1.0f, 0.0f) * deltaTime);
}

void DuckEntity::beginPlay() {
    Entity::beginPlay();

    auto& EntityStaticMesh = this->getComponent<StaticMeshComponent>();
    // Ensure the visual mesh is centered on the entity, can remove this later
    EntityStaticMesh.setPosition(glm::vec3(0.0f));

    world->CreateCube(EntityStaticMesh.VAO, EntityStaticMesh.VBO);
}