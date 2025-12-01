#include "BoundsSystem.h"
#include "../ecs/World.h"
#include "../ecs/ECS.h"
#include "../ecs/components/Transform.h"
#include "../ecs/components/BoundsComponent.h"
#include "../core/managers/GameStateManager.h"

void BoundsSystem::update(World& world, float deltaTime) {
    for (EntityID eid : world.registry.getEntitiesWith<BoundsComponent>()) {
        if (!world.registry.hasComponent<Transform>(eid)) continue;

        auto& transform = world.registry.getComponent<Transform>(eid);
        auto& bounds = world.registry.getComponent<BoundsComponent>(eid);

        if (glm::distance(transform.position, bounds.spawnPosition) > bounds.escapeDistance) {
            GameStateManager::get().duckEscaped();
            world.registry.destroyEntity(eid);
        }
    }
}
