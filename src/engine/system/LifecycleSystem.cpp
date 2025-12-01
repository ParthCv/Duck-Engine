#include "LifecycleSystem.h"
#include "../ecs/World.h"
#include "../ecs/components/Transform.h"
#include "../ecs/components/Velocity.h"
#include "../ecs/components/HealthComponent.h"
#include "../ecs/components/StaticMeshComponent.h"
#include "../core/managers/GameStateManager.h"
#include "../core/managers/AudioManager.h"
#include "../core/managers/ResourceManager.h"
#include <iostream>

void LifecycleSystem::update(World& world, float deltaTime) {
    for (EntityID eid : world.registry.getEntitiesWith<HealthComponent>()) {
        if (!world.registry.hasComponent<Transform>(eid)) continue;

        auto& health = world.registry.getComponent<HealthComponent>(eid);
        auto& transform = world.registry.getComponent<Transform>(eid);

        if (health.isDead) {
            health.timeSinceDeath += deltaTime;

            // Wait for the "pause" to finish, then start falling
            if (health.timeSinceDeath > health.pauseAfterKillDuration && !health.isFalling) {
                health.isFalling = true;

                if(world.registry.hasComponent<Velocity>(eid)) {
                    auto& velocity = world.registry.getComponent<Velocity>(eid);

                    // FIX: Simplified Falling Logic
                    // Since MovementSystem is now World Space, we just set direction to World Down.
                    velocity.Direction = glm::vec3(0.0f, -1.0f, 0.0f);
                    velocity.Speed = 50.0f; // Set a fast fall speed
                }
            }
        }

        // Destroy if below the death plane
        if (transform.position.y < health.DeathPlaneYBound) {
            world.registry.destroyEntity(eid);
        }
    }
}

void LifecycleSystem::killDuck(World& world, EntityID eid) {
    if (!world.registry.hasComponent<HealthComponent>(eid)) return;

    auto& health = world.registry.getComponent<HealthComponent>(eid);
    if (health.isDead) return;

    health.isDead = true;
    AudioManager::Get().PlaySound("quack");
    GameStateManager::get().hitDuck();
    std::cout << "Duck Died" << std::endl;

    if (world.registry.hasComponent<Velocity>(eid)) {
        auto& velocity = world.registry.getComponent<Velocity>(eid);
        // Stop the bird mid-air
        velocity.Direction = glm::vec3(0.0f);
        velocity.Speed = 0.0f;
    }

    // Keep the rotation as-is so the duck doesn't visually "jump" when killed
    // The duck will fall in whatever orientation it had when shot (more natural)

    if (world.registry.hasComponent<StaticMeshComponent>(eid) && world.registry.hasComponent<Transform>(eid)) {
        auto& mesh = world.registry.getComponent<StaticMeshComponent>(eid);
        auto& transform = world.registry.getComponent<Transform>(eid);

        mesh.Mesh = ResourceManager::Get().GetStaticMesh("turkey.obj");
        mesh.material = ResourceManager::Get().GetMaterial("turkey");

        // Scale down the turkey model to match the duck size (turkey model is larger)
        transform.scale = glm::vec3(2.0f);
    }
}