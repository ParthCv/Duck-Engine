#include "BoundsSystem.h"
#include "../ecs/World.h"
#include "../ecs/Entity.h"
#include "../ecs/components/Transform.h"
#include "../ecs/components/BoundsComponent.h"
#include "../game/DuckGameState.h"

void BoundsSystem::update(World& world, float deltaTime) {
    for (auto& entity : world.EntityManager.GetEntities()) {
        if (entity->hasComponent<Transform>() && entity->hasComponent<BoundsComponent>()) {
            auto& transform = entity->getComponent<Transform>();
            auto& bounds = entity->getComponent<BoundsComponent>();

            if (glm::distance(transform.position, bounds.spawnPosition) > bounds.escapeDistance) {
                DuckGameState::get().incrementDucksEscaped();
                entity->destroy();
            }
        }
    }
}
