#include "LifecycleSystem.h"
#include "../src/engine/ecs/World.h"
#include "../src/engine/ecs/Entity.h"
#include "../src/engine/ecs/components/Transform.h"
#include "../src/engine/ecs/components/Velocity.h"
#include "../src/engine/ecs/components/HealthComponent.h"
#include "../src/engine/ecs/components/StaticMeshComponent.h"
#include "../components/GameRoundComponent.h"
#include "../src/engine/core/managers/AudioManager.h"
#include "../src/engine/core/managers/ResourceManager.h"
#include <iostream>

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

void LifecycleSystem::killDuck(Entity& entity) {
    if (!entity.hasComponent<HealthComponent>()) return;

    auto& health = entity.getComponent<HealthComponent>();
    if (health.isDead) return;

    health.isDead = true;
    AudioManager::Get().PlaySound("quack");

    // === UPDATE GAME STATE USING COMPONENTS ===
    // NOTE: Game state update is now handled by ShootingSystem
    // We don't need to update it here - just handle the visual/audio effects

    std::cout << "Duck Died" << std::endl;

    if (entity.hasComponent<Velocity>()) {
        auto& velocity = entity.getComponent<Velocity>();
        // Stop the bird mid-air
        velocity.Direction = glm::vec3(0.0f);
        velocity.Speed = 0.0f;
    }

    // Keep the rotation as-is so the duck doesn't visually "jump" when killed
    // The duck will fall in whatever orientation it had when shot (more natural)

    if (entity.hasComponent<StaticMeshComponent>() && entity.hasComponent<Transform>()) {
        auto& mesh = entity.getComponent<StaticMeshComponent>();
        auto& transform = entity.getComponent<Transform>();

        mesh.Mesh = ResourceManager::Get().GetStaticMesh("turkey.obj");
        mesh.material = ResourceManager::Get().GetMaterial("turkey");

        // Scale down the turkey model to match the duck size (turkey model is larger)
        transform.scale = glm::vec3(2.0f);
    }
}