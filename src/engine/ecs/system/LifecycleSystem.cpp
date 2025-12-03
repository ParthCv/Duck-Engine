#include "LifecycleSystem.h"
#include "../src/engine/ecs/World.h"
#include "../src/engine/ecs/Entity.h"
#include "../src/engine/ecs/components/Transform.h"
#include "../src/engine/ecs/components/Velocity.h"
#include "../src/engine/ecs/components/HealthComponent.h"

void LifecycleSystem::update(World& world, float deltaTime) {
    for (auto& entity : world.EntityManager.GetEntities()) {
        if (entity->hasComponent<HealthComponent>() && entity->hasComponent<Transform>()) {
            auto& health = entity->getComponent<HealthComponent>();
            auto& transform = entity->getComponent<Transform>();

            if (health.isDead) {
                health.timeSinceDeath += deltaTime;

                // Wait for the "pause" to finish, then start falling
                if (health.timeSinceDeath > health.pauseAfterKillDuration && !health.isFalling) {
                    health.isFalling = true;

                    if(entity->hasComponent<Velocity>()) {
                        auto& velocity = entity->getComponent<Velocity>();

                        // FIX: Simplified Falling Logic
                        // Since MovementSystem is now World Space, we just set direction to World Down.
                        velocity.Direction = glm::vec3(0.0f, -1.0f, 0.0f);
                        velocity.Speed = 50.0f; // Set a fast fall speed
                    }
                }
            }

            // Destroy if below the death plane
            if (transform.position.y < health.DeathPlaneYBound) {
                entity->destroy();
            }
        }
    }
}

void LifecycleSystem::killEntity(Entity& entity) {
    if (!entity.hasComponent<HealthComponent>()) return;

    auto& health = entity.getComponent<HealthComponent>();
    if (health.isDead) return;

    // Set death flag
    health.isDead = true;

    // Stop movement (generic death behavior)
    if (entity.hasComponent<Velocity>()) {
        auto& velocity = entity.getComponent<Velocity>();
        velocity.Direction = glm::vec3(0.0f);
        velocity.Speed = 0.0f;
    }

    // Keep the rotation as-is so the entity doesn't visually "jump" when killed
    // Game-specific systems (like DuckDeathSystem) should handle visual transformations
}