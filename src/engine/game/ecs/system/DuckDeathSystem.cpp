#include "DuckDeathSystem.h"
#include "../../../ecs/World.h"
#include "../../../ecs/Entity.h"
#include "../../../ecs/components/Transform.h"
#include "../../../ecs/components/HealthComponent.h"
#include "../../../ecs/components/StaticMeshComponent.h"
#include "../../../ecs/components/DuckComponent.h"
#include "../../../core/managers/GameStateManager.h"
#include "../../../core/managers/AudioManager.h"
#include "../../../core/managers/ResourceManager.h"
#include <iostream>

void DuckDeathSystem::update(World& world, float deltaTime) {
    // Find all dead ducks that haven't been visually transformed yet
    auto ducks = world.EntityManager.GetEntitiesWith<DuckComponent, HealthComponent, StaticMeshComponent, Transform>();

    for (auto* duck : ducks) {
        auto& health = duck->getComponent<HealthComponent>();

        // If the duck just died (timeSinceDeath is very small), apply death effects
        if (health.isDead && health.timeSinceDeath < 0.01f) {
            handleDuckDeath(*duck);
        }
    }
}

void DuckDeathSystem::handleDuckDeath(Entity& duck) {
    // Play duck death sound
    AudioManager::Get().PlaySound("quack");

    // Update game state (score, UI, etc.)
    GameStateManager::get().hitDuck();

    std::cout << "Duck Died" << std::endl;

    // Apply visual transformation: duck -> turkey
    if (duck.hasComponent<StaticMeshComponent>() && duck.hasComponent<Transform>()) {
        auto& mesh = duck.getComponent<StaticMeshComponent>();
        auto& transform = duck.getComponent<Transform>();

        mesh.Mesh = ResourceManager::Get().GetStaticMesh("turkey.obj");
        mesh.material = ResourceManager::Get().GetMaterial("turkey");

        // Scale down the turkey model to match the duck size (turkey model is larger)
        transform.scale = glm::vec3(2.0f);
    }
}
