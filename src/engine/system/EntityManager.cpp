//
// Created by super on 2025-11-17.
//

#include "../ecs/Entity.h"
#include "EntityManager.h"

EntityManager::EntityManager()
{
}

void EntityManager::BeginPlay()
{
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

void EntityManager::Update()
{
    // TODO: Do all the updating below.
    // ....

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