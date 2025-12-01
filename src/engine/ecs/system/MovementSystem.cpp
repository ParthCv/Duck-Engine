#include "MovementSystem.h"
#include "../src/engine/ecs/World.h"
#include "../src/engine/ecs/Entity.h"
#include "../src/engine/ecs/components/Transform.h"
#include "../src/engine/ecs/components/Velocity.h"

void MovementSystem::update(World& world, float deltaTime) {
    for (auto& entity : world.EntityManager.GetEntities()) {
        if (entity->hasComponent<Transform>() && entity->hasComponent<Velocity>()) {
            auto& transform = entity->getComponent<Transform>();
            auto& velocity = entity->getComponent<Velocity>();

            transform.position += velocity.Direction * velocity.Speed * deltaTime;

            if (glm::length(velocity.Direction) > 0.01f) {
                transform.rotation = glm::quatLookAt(glm::normalize(-velocity.Direction), glm::vec3(0, 1, 0));
            }
        }
    }
}