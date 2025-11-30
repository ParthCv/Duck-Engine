#include "LifecycleSystem.h"
#include "../ecs/World.h"
#include "../ecs/Entity.h"
#include "../ecs/components/Transform.h"
#include "../ecs/components/Velocity.h"
#include "../ecs/components/HealthComponent.h"
#include "../ecs/components/StaticMeshComponent.h"
#include "../game/DuckGameState.h"
#include "../core/managers/AudioManager.h"
#include "../core/managers/ResourceManager.h"
#include <iostream>

void LifecycleSystem::update(World& world, float deltaTime) {
    for (auto& entity : world.EntityManager.GetEntities()) {
        if (entity->hasComponent<HealthComponent>() && entity->hasComponent<Transform>()) {
            auto& health = entity->getComponent<HealthComponent>();
            auto& transform = entity->getComponent<Transform>();

            if (health.isDead) {
                health.timeSinceDeath += deltaTime;
                if (health.timeSinceDeath > health.pauseAfterKillDuration && !health.isFalling) {
                    health.isFalling = true;
                    
                    auto& velocity = entity->getComponent<Velocity>();
                    // Calculate local velocity needed to fall "world" down
                    glm::quat inverseRotation = glm::inverse(transform.rotation);
                    glm::vec3 localDown = inverseRotation * glm::vec3(0.0f, -1.0f, 0.0f);
                    glm::vec3 fallVelocity = localDown * 0.5f;
                    velocity.Direction = fallVelocity;
                    velocity.Speed = 1.0f;
                }
            }

            if (transform.position.y < health.DeathPlaneYBound) {
                entity->destroy();
            }
        }
    }
}

void LifecycleSystem::killDuck(Entity& entity) {
    if (!entity.hasComponent<HealthComponent>()) return;

    auto& health = entity.getComponent<HealthComponent>();
    if (health.isDead) return;

    health.isDead = true;
    AudioManager::Get().PlaySound("quack");
    DuckGameState::get().hitDuck();
    std::cout << "Duck Died" << std::endl;

    if (entity.hasComponent<Velocity>()) {
        auto& velocity = entity.getComponent<Velocity>();
        velocity.Direction = glm::vec3(0.0f);
        velocity.Speed = 0.0f;
    }

    if (entity.hasComponent<Transform>()) {
        auto& transform = entity.getComponent<Transform>();
        transform.rotation = glm::quat(); // Reset rotation
    }

    if (entity.hasComponent<StaticMeshComponent>()) {
        auto& mesh = entity.getComponent<StaticMeshComponent>();
        mesh.Mesh = ResourceManager::Get().GetStaticMesh("turkey.obj");
        mesh.material = ResourceManager::Get().GetMaterial("turkey");
    }
}
