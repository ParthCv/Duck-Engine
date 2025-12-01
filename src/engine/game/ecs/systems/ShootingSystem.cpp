#include "ShootingSystem.h"
#include "../src/engine/ecs/World.h"
#include "../src/engine/ecs/Component.h"
#include "../../../ecs/components/GameRoundComponent.h"
#include "../src/engine/core/managers/InputManager.h"
#include "../src/engine/core/managers/AudioManager.h"
#include "../src/engine/game/EventQueue.h"
#include "GunSystem.h"
#include "GLFW/glfw3.h"

void ShootingSystem::update(World& world, float deltaTime) {
    // Get player entities with raycast capability
    auto raycastEntities = world.EntityManager.GetEntitiesWith<RaycastSource, Transform>();
    
    // Get game entity with ammo and score
    auto gameEntities = world.EntityManager.GetEntitiesWith<AmmoComponent, ScoreComponent, GameRoundComponent, DuckUIStateComponent>();
    
    if (raycastEntities.empty() || gameEntities.empty()) {
        return;
    }
    
    Entity* playerEntity = raycastEntities[0];
    Entity* gameEntity = gameEntities[0];
    
    // Check for shoot input
    if (InputManager::isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
        auto& ammo = gameEntity->getComponent<AmmoComponent>();
        
        if (ammo.hasAmmo()) {
            processShot(world, *playerEntity, *gameEntity);
        }
    }
}

void ShootingSystem::processShot(World& world, Entity& playerEntity, Entity& gameEntity) {
    auto& ammo = gameEntity.getComponent<AmmoComponent>();
    auto& score = gameEntity.getComponent<ScoreComponent>();
    auto& roundComp = gameEntity.getComponent<GameRoundComponent>();
    auto& uiState = gameEntity.getComponent<DuckUIStateComponent>();
    auto& raySource = playerEntity.getComponent<RaycastSource>();
    
    // Consume ammo
    ammo.consume();
    
    // Play sound
    AudioManager::Get().PlaySound("shoot", 0.5f);
    
    // Apply recoil to gun entities
    auto gunEntities = world.EntityManager.GetEntitiesWith<GunComponent, Transform>();
    for (auto* gunEntity : gunEntities) {
        GunSystem::applyRecoil(*gunEntity);
    }
    
    // Set raycast direction from camera (FPS style - always forward)
    raySource.direction = world.camera->front;
    raySource.drawRay = true;
    
    // Perform raycast
    if (world.collisionSystem) {
        auto result = world.collisionSystem->RaycastFromEntity(world.EntityManager, playerEntity);
        
        if (result.hit && result.hitEntity && result.hitEntity->hasComponent<HealthComponent>()) {
            // Hit a duck!
            world.lifecycleSystem.killDuck(*result.hitEntity);
            
            // Award points
            int points = score.calculateDuckPoints(roundComp.currentRound);
            score.totalScore += points;
            
            // Update UI state
            if (roundComp.ducksResolved < roundComp.maxDucksPerRound) {
                uiState.states[roundComp.ducksResolved] = DuckState::HIT;
                roundComp.ducksResolved++;
            }
            
            // Emit event for UI
            world.events.emit(DuckShotEvent{roundComp.ducksResolved - 1, points});
        } else {
            // Missed
            // Could emit a miss event here if needed
        }
    }
    
    // Emit bullet fired event
    world.events.emit(BulletFiredEvent{ammo.current});
}
