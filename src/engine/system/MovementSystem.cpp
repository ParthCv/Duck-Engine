#include "MovementSystem.h"
#include "../ecs/ECS.h"
#include "../ecs/World.h"
#include "../ecs/components/Transform.h"
#include "../ecs/components/Velocity.h"

void MovementSystem::update(World& world, float deltaTime) {
    for (EntityID eid : world.registry.getEntitiesWith<Velocity>()) {
        if (!world.registry.hasComponent<Transform>(eid)) continue;

        auto& transform = world.registry.getComponent<Transform>(eid);
        auto& velocity = world.registry.getComponent<Velocity>(eid);

        transform.position += velocity.Direction * velocity.Speed * deltaTime;

        if (glm::length(velocity.Direction) > 0.01f) {
            transform.rotation = glm::quatLookAt(glm::normalize(-velocity.Direction), glm::vec3(0, 1, 0));
        }
    }
}