#include "GameRoundSystem.h"
#include "../src/engine/ecs/World.h"
#include "../../../ecs/components/GameRoundComponent.h"
#include "../src/engine/game/EventQueue.h"
#include "../src/engine/core/managers/AudioManager.h"

void GameRoundSystem::update(World& world, float deltaTime) {
    auto gameEntities = world.EntityManager.GetEntitiesWith<GameRoundComponent, AmmoComponent, ScoreComponent, DuckUIStateComponent>();
    
    if (gameEntities.empty()) {
        return;
    }
    
    Entity* gameEntity = gameEntities[0];
    
    checkRoundCompletion(world, *gameEntity);
}

void GameRoundSystem::checkRoundCompletion(World& world, Entity& gameEntity) {
    auto& roundComp = gameEntity.getComponent<GameRoundComponent>();
    auto& scoreComp = gameEntity.getComponent<ScoreComponent>();
    
    if (roundComp.isRoundComplete()) {
        if (roundComp.hasRoundFailed()) {
            AudioManager::Get().PlaySound("lose", 1.0f);
            endGame(gameEntity, world, false);
        } else {
            AudioManager::Get().PlaySound("win", 1.0f);
            startNextRound(gameEntity, world);
        }
    }
}

void GameRoundSystem::startNextRound(Entity& gameEntity, World& world) {
    auto& roundComp = gameEntity.getComponent<GameRoundComponent>();
    auto& ammoComp = gameEntity.getComponent<AmmoComponent>();
    auto& uiStateComp = gameEntity.getComponent<DuckUIStateComponent>();
    
    // Increment round
    roundComp.currentRound++;
    
    // Reset round counters
    roundComp.ducksSpawned = 0;
    roundComp.ducksResolved = 0;
    roundComp.ducksEscaped = 0;
    
    // Refill ammo
    ammoComp.refill();
    
    // Reset UI state
    uiStateComp.resetStates();
    
    // Emit round start event
    world.events.emit(RoundStartEvent{roundComp.currentRound, roundComp.maxDucksPerRound});
}

void GameRoundSystem::endGame(Entity& gameEntity, World& world, bool victory) {
    auto& roundComp = gameEntity.getComponent<GameRoundComponent>();
    auto& scoreComp = gameEntity.getComponent<ScoreComponent>();
    
    // Emit game over event
    world.events.emit(GameOverEvent{victory, scoreComp.totalScore, roundComp.currentRound});
}
