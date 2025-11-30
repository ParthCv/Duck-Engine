#include "DuckFactory.h"
#include "../ecs/World.h"
#include "../ecs/Entity.h"
#include "../ecs/Component.h"
#include "../system/TransformSystem.h"
#include "../core/managers/ResourceManager.h"
#include "../ecs/components/Transform.h"
#include "../ecs/components/Velocity.h"
#include "../ecs/components/StaticMeshComponent.h"
#include "../ecs/components/BoxCollider.h"
#include "../ecs/components/DebugDrawable.h"
#include "../ecs/components/DuckComponent.h"
#include "../ecs/components/HealthComponent.h"
#include "../ecs/components/BoundsComponent.h"
#include "../ecs/components/ScoreValueComponent.h"
#include <cstdlib>

Entity* DuckFactory::createDuck(World& world, const glm::vec3& position, float speed) {
    auto& entity = world.EntityManager.CreateEntity(world);

    // Add components
    auto& transform = entity.addComponent<Transform>();
    transform.position = position;
    transform.scale = glm::vec3(10.0f);

    auto& velocity = entity.addComponent<Velocity>();
    velocity.Speed = speed;

    auto& staticMesh = entity.addComponent<StaticMeshComponent>();
    staticMesh.Mesh = ResourceManager::Get().GetStaticMesh("duck.obj");
    staticMesh.material = ResourceManager::Get().GetMaterial("duck");

    auto& collider = entity.addComponent<BoxCollider>();
    collider.size = staticMesh.Mesh->getSize(); // Store unscaled size (CollisionSystem will apply transform.scale)
    collider.center = staticMesh.Mesh->getCenter();

    // Adjust center offset based on unscaled size
    collider.center.y += (collider.size.y * transform.scale.y) / 2.0f;

    entity.addComponent<DebugDrawable>();
    entity.addComponent<DuckComponent>();
    entity.addComponent<HealthComponent>();
    
    auto& bounds = entity.addComponent<BoundsComponent>();
    bounds.spawnPosition = position;

    entity.addComponent<ScoreValueComponent>();

    // Set random flight path
    TransformSystem::LocalRotate(transform, -45.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    float randomAngle = rand() % 360;
    glm::quat rotation = glm::quat(glm::vec3(glm::radians(-45.0f), glm::radians(randomAngle), 0.0f));
    velocity.Direction = rotation * glm::vec3(0.0f, 0.0f, 10.0f);


    return &entity;
}
