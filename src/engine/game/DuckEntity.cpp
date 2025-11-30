#include "DuckEntity.h"
#include "../ecs/World.h"
#include "../ecs/Component.h"
#include "GLFW/glfw3.h"
#include <cstdlib>
#include <iostream>
#include "../core/managers/AudioManager.h"
#include "../core/managers/GameStateManager.h"

DuckEntity::DuckEntity(World& InWorld) : Entity(InWorld)
{
    auto& transform = addComponent<Transform>(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(10,10,10));

    // Initialize Velocity
    addComponent<Velocity>(glm::vec3(1.0, 0.0f, 0.0f), 0.25f);

    // Initialize Collider
    auto& collider = addComponent<BoxCollider>();

    // Initialize Mesh
    auto& staticMeshComponent = addComponent<StaticMeshComponent>(*this);
    staticMeshComponent.loadMesh("duck.obj");

    staticMeshComponent.material = ResourceManager::Get().GetMaterial("duck");

    // Configure Collider based on Mesh
    collider.size = staticMeshComponent.Mesh->getSize() * transform.scale;
    collider.center = staticMeshComponent.Mesh->getCenter();
    collider.center.y += collider.size.y / 2.0f;

    // Initialize Debug
    auto& debug = addComponent<DebugDrawable>();
    debug.drawCollider = true;
    debug.colliderColor = glm::vec3(1.0f, 0.0f, 0.0f); // Red

    this->DuckEntity::beginPlay();
}

DuckEntity::DuckEntity(World &InWorld, glm::vec3 &InPosition) : Entity(InWorld) {
    auto& transform = addComponent<Transform>(InPosition, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(10,10,10));

    spawnPosition = InPosition;

    addComponent<Velocity>(glm::vec3(0.0, 0.0f, 0.0f), 0.0f);
    addComponent<StaticMeshComponent>(*this );

    // Initialize Mesh
    auto& staticMeshComponent = addComponent<StaticMeshComponent>(*this);
    staticMeshComponent.loadMesh("duck.obj");

    // material.loadAlbedoMap("../assets/textures/duck.png");
    // material.setMetallic(0.0f);
    // material.setRoughness(0.8f);

    // staticMeshComponent.material = &material;
    staticMeshComponent.material = ResourceManager::Get().GetMaterial("duck");
    // Initialize Collider
    auto& collider = addComponent<BoxCollider>();
    collider.size = staticMeshComponent.Mesh->getSize() * transform.scale;
    collider.center = staticMeshComponent.Mesh->getCenter();
    collider.center.y += collider.size.y / 2.0f;

    auto& debug = addComponent<DebugDrawable>();
    debug.drawCollider = true;
    debug.colliderColor = glm::vec3(1.0f, 0.0f, 0.0f);

    this->DuckEntity::beginPlay();
}

DuckEntity::DuckEntity(World &InWorld, glm::vec3 &InPosition, float speed) : Entity(InWorld) {
    auto& transform = addComponent<Transform>(InPosition, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(10,10,10));

    spawnPosition = InPosition;

    addComponent<Velocity>(glm::vec3(0.0, 0.0f, 1.0f), speed);
    addComponent<StaticMeshComponent>(*this );

    // Initialize Mesh
    auto& staticMeshComponent = addComponent<StaticMeshComponent>(*this);
    staticMeshComponent.loadMesh("duck.obj");

    // Initialize Collider
    auto& collider = addComponent<BoxCollider>();
    collider.size = staticMeshComponent.Mesh->getSize() * transform.scale;
    collider.center = staticMeshComponent.Mesh->getCenter();
    collider.center.y += collider.size.y / 2.0f;

    auto& debug = addComponent<DebugDrawable>();
    debug.drawCollider = true;
    debug.colliderColor = glm::vec3(1.0f, 0.0f, 0.0f);

    this->DuckEntity::beginPlay();
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

    if (!isDead)
        checkIfEscaped();
    else {
        timeSinceDeath += deltaTime;
        if (timeSinceDeath > pauseAfterKillDuration && !isFalling) {
            isFalling = true;
            fall();
        }
    }

    // Destroy Entity if it sinks low enough
    if (EntityTransform.position.y < DeathPlaneYBound) {
        this->destroy();
    }
}

void DuckEntity::beginPlay() {
    Entity::beginPlay();
    // Mesh loading moved to constructor

    setRandomFlightPath();
}

void DuckEntity::setRandomFlightPath() {

    auto& EntityTransform = this->getComponent<Transform>();
    auto& EntityStaticMesh = this->getComponent<StaticMeshComponent>();
    auto& EntityVelocity = this->getComponent<Velocity>();

    // TODO: Rotate to face 45 degrees up into the air
    EntityTransform.LocalRotate(-45.0f, glm::vec3(1.0f, 0.0f, 0.0f));

    // bool moveRight = EntityTransform.position.x < 0.0f;
    // float dirX = moveRight ? 1.0f : -1.0f;

    // // Rotate to face the flight direction
    // EntityTransform.rotation = glm::quat(glm::vec3(0.0f));
    // float rotationAngle = moveRight ? 90.0f : -90.0f;
    // EntityTransform.WorldRotate(rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));

    // // TODO: Set Entity Velocity
    // EntityVelocity.setVelocity(glm::vec3(dirX, 0.0f, 1.0f), 1.0f);

    // TODO: Rotate around Y-Axis randomly
    float randomAngle = rand() % 360;
    EntityTransform.WorldRotate(randomAngle, glm::vec3(0.0f, 1.0f, 0.0f));

    // TODO: Set Entity Velocity
    EntityVelocity.setVelocity(glm::vec3(0.0f, 0.0f, 1.0f), 0.1f);
}

void DuckEntity::checkIfEscaped()
{
    auto& EntityTransform = this->getComponent<Transform>();
    // Increased escape distance slightly since they spawn further out
    if (glm::distance(EntityTransform.position, spawnPosition) > escapeDistance)
    {
        GameStateManager::get().duckEscaped(0.0f, 0.0f);
        this->destroy();
    }
}

void DuckEntity::KillDuck() {
    if (isDead)
        return;
    AudioManager::Get().PlaySound("quack");

    // Increase score and decrement numOfDucks
    GameStateManager::get().hitDuck(0.0f, 0.0f);  // Marks next slot as HIT

    auto& EntityTransform = this->getComponent<Transform>();
    auto& EntityVelocity = this->getComponent<Velocity>();
    EntityTransform.SetRotation(glm::vec3(0, EntityTransform.rotation.y, 0));
    EntityVelocity.setVelocity(glm::vec3(0.0f, 0.0f, 0.0f), 0.0f);
    cook();  // Change static mesh
    isDead = true;
}

void DuckEntity::fall() {
    auto& EntityTransform = this->getComponent<Transform>();
    auto& EntityVelocity = this->getComponent<Velocity>();

    // Calculate local velocity needed to fall "world" down
    glm::quat inverseRotation = glm::inverse(EntityTransform.rotation);
    glm::vec3 localDown = inverseRotation * glm::vec3(0.0f, -1.0f, 0.0f);
    glm::vec3 fallVelocity = localDown * 0.5f;

    EntityVelocity.setVelocity(fallVelocity, 1.0f);
}

// Change static mesh on death
void DuckEntity::cook() {
    auto& staticMeshComponent = addComponent<StaticMeshComponent>(*this);

    staticMeshComponent.loadMesh("turkey.obj");
    staticMeshComponent.material = ResourceManager::Get().GetMaterial("turkey");
    staticMeshComponent.StaticMeshTransform.scale = glm::vec3(0.2f, 0.2f, 0.2f);
    staticMeshComponent.StaticMeshTransform.SetTransform(glm::vec3(0.0f, 0.1f, 0.0f));
}