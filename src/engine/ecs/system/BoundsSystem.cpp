#include "BoundsSystem.h"
#include "../src/engine/ecs/World.h"
#include "../src/engine/ecs/Entity.h"
#include "../src/engine/ecs/components/Transform.h"
#include "../src/engine/ecs/components/BoundsComponent.h"
#include "../components/GameRoundComponent.h"

void BoundsSystem::update(World& world, float deltaTime) {
    // Get the game entity (holds game state)
    Entity* gameEntity = world.getGameEntity();

    for (auto& entity : world.EntityManager.GetEntities()) {
        if (entity->hasComponent<Transform>() && entity->hasComponent<BoundsComponent>()) {
            auto& transform = entity->getComponent<Transform>();
            auto& bounds = entity->getComponent<BoundsComponent>();

            if (glm::distance(transform.position, bounds.spawnPosition) > bounds.escapeDistance) {
                // === UPDATE GAME STATE USING COMPONENTS ===
                if (gameEntity) {
                    auto& roundComp = gameEntity->getComponent<GameRoundComponent>();
                    auto& uiState = gameEntity->getComponent<DuckUIStateComponent>();

                    // Only update if we haven't resolved all ducks yet
                    if (roundComp.ducksResolved < roundComp.maxDucksPerRound) {
                        // Increment escape counter
                        roundComp.ducksEscaped++;

                        // Update UI state
                        uiState.states[roundComp.ducksResolved] = DuckState::ESCAPED;

                        // Increment resolved counter
                        roundComp.ducksResolved++;

                        // Emit event for UI/audio
                        world.events.emit(DuckEscapedEvent{roundComp.ducksResolved - 1});
                    }
                }

                entity->destroy();
            }
        }
    }
}