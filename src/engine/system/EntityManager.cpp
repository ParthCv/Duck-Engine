//
// Created by super on 2025-11-17.
//

#include "../ecs/Entity.h"
#include "EntityManager.h"

#include "../ecs/Component.h"
#include "../game/DuckEntity.h"

EntityManager::EntityManager()
{
}

void EntityManager::BeginPlay()
{
    // TODO: Call BeginPlay on all entities
    for (auto& entity : Entities)
    {
        entity->BeginPlay();
    }
}

std::vector<std::unique_ptr<Entity>>& EntityManager::GetEntities()
{
    return Entities;
}

Entity& EntityManager::CreateEntity(World& InWorld)
{
    Entities.emplace_back(std::make_unique<Entity>(InWorld));
    return *Entities.back();
}

DuckEntity& EntityManager::CreateDuckEntity(World &InWorld)
{
    Entities.emplace_back(std::make_unique<DuckEntity>(InWorld));
    return dynamic_cast<DuckEntity&>(*Entities.back());
}

Entity& EntityManager::CreateDeferredEntity(World& InWorld)
{
    DeferredEntities.emplace_back(std::make_unique<Entity>(InWorld));
    return *DeferredEntities.back();
}

void EntityManager::SynchronizeEntities()
{
    if (!DeferredEntities.empty()) {
        std::move(DeferredEntities.begin(), DeferredEntities.end(), std::back_inserter(Entities));
        DeferredEntities.clear();
    }
}

void EntityManager::Update(float deltaTime)
{
    // TODO: Do all the updating below.
    for (auto& entity : Entities) {
        // TODO Update the Entity itself.
        entity->Update(deltaTime);

        // TODO: Update ticking components
        entity->GetComponent<StaticMeshComponent>().Update(deltaTime);
    }

    // TODO: Cleanup at the end.
    CleanupInactiveEntities();
}

void EntityManager::CleanupInactiveEntities()
{
    std::erase_if(
        Entities,
        [](std::unique_ptr<Entity>& e) {
            return !e->bIsActive;
        });
}