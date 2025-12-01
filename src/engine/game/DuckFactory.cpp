#include "DuckFactory.h"
#include "../ecs/World.h"
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

EntityID DuckFactory::createDuck(World& world, const glm::vec3& position, float speed) {
    EntityID eid = world.registry.createEntity();

    // Add components
    Transform transform;
    transform.position = position;
    transform.scale = glm::vec3(10.0f);
    world.registry.addComponent(eid, transform);

    Velocity velocity;
    velocity.Speed = speed;
    world.registry.addComponent(eid, velocity);

    StaticMeshComponent staticMeshComponent;
    staticMeshComponent.Mesh = ResourceManager::Get().GetStaticMesh("duck.obj");
    staticMeshComponent.material = ResourceManager::Get().GetMaterial("duck");
    world.registry.addComponent(eid, staticMeshComponent);

    BoxCollider collider;
    collider.size = staticMeshComponent.Mesh->getSize(); // Store unscaled size (CollisionSystem will apply transform.scale)
    collider.center = staticMeshComponent.Mesh->getCenter();
    collider.center.y += (collider.size.y * transform.scale.y) / 2.0f; // Adjust center offset based on unscaled size
    world.registry.addComponent(eid, collider);

    world.registry.addComponent(eid, DebugDrawable{});
    world.registry.addComponent(eid, DuckComponent{});
    world.registry.addComponent(eid, HealthComponent{});

    BoundsComponent bounds;
    bounds.spawnPosition = position;
    world.registry.addComponent(eid, bounds);

    world.registry.addComponent(eid, ScoreValueComponent{});

    // Set random flight path
    TransformSystem::LocalRotate(transform, -45.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    float randomAngle = rand() % 360;
    glm::quat rotation = glm::quat(glm::vec3(glm::radians(-45.0f), glm::radians(randomAngle), 0.0f));
    velocity.Direction = rotation * glm::vec3(0.0f, 0.0f, 10.0f);


    return eid;
}
