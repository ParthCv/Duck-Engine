#include "BoundsSystem.h"
#include "../src/engine/ecs/World.h"
#include "../src/engine/ecs/Entity.h"
#include "../src/engine/ecs/components/Transform.h"
#include "../src/engine/ecs/components/BoundsComponent.h"
#include "../src/engine/game/ecs/system/GameStateSystem.h"

void BoundsSystem::update(World& world, float deltaTime) {
    // Get game state entity for escape tracking
    Entity* gameState = world.getGameStateEntity();
    if (!gameState) return;

    for (auto& entity : world.EntityManager.GetEntities()) {
        if (entity->hasComponent<Transform>() && entity->hasComponent<BoundsComponent>()) {
            auto& transform = entity->getComponent<Transform>();
            auto& bounds = entity->getComponent<BoundsComponent>();

            if (glm::distance(transform.position, bounds.spawnPosition) > bounds.escapeDistance) {
                GameStateSystem::duckEscaped(*gameState);
                entity->destroy();
            }
        }
    }
}
