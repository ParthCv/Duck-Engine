#include "BoundsSystem.h"
#include "../src/engine/ecs/World.h"
#include "../src/engine/ecs/Entity.h"
#include "../src/engine/ecs/components/Transform.h"
#include "../src/engine/ecs/components/BoundsComponent.h"
#include "../src/engine/core/managers/GameStateManager.h"

void BoundsSystem::update(World& world, float deltaTime) {
    for (auto& entity : world.EntityManager.GetEntities()) {
        if (entity->hasComponent<Transform>() && entity->hasComponent<BoundsComponent>()) {
            auto& transform = entity->getComponent<Transform>();
            auto& bounds = entity->getComponent<BoundsComponent>();

            if (glm::distance(transform.position, bounds.spawnPosition) > bounds.escapeDistance) {
                GameStateManager::get().duckEscaped();
                entity->destroy();
            }
        }
    }
}
