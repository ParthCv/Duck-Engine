#include "MovementSystem.h"
#include "../ecs/World.h"
#include "../ecs/Entity.h"
#include "../ecs/components/Transform.h"
#include "../ecs/components/Velocity.h"
#include "TransformSystem.h"

void MovementSystem::update(World& world, float deltaTime) {
    for (auto& entity : world.EntityManager.GetEntities()) {
        if (entity->hasComponent<Transform>() && entity->hasComponent<Velocity>()) {
            auto& transform = entity->getComponent<Transform>();
            auto& velocity = entity->getComponent<Velocity>();

            TransformSystem::AddLocalTransform(transform, velocity.Direction * velocity.Speed * deltaTime);
        }
    }
}
