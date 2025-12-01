#include "EntityManager.h"
#include "../ecs/ECS.h"
#include "../ecs/World.h"
#include "../ecs/Component.h"

EntityManager::EntityManager() = default;

void EntityManager::BeginPlay()
{
}

std::vector<EntityID>& EntityManager::GetEntities(World& InWorld)
{
    return InWorld.registry.getAllEntities();
}

EntityID EntityManager::CreateEntity(World& InWorld)
{
    return InWorld.registry.createEntity();
}

//Entity& EntityManager::CreateDeferredEntity(World& InWorld)
//{
//    DeferredEntities.emplace_back(std::make_unique<Entity>(InWorld));
//    return *DeferredEntities.back();
//}
//
//
//void EntityManager::SynchronizeEntities()
//{
//    if (!DeferredEntities.empty()) {
//        std::move(DeferredEntities.begin(), DeferredEntities.end(), std::back_inserter(Entities));
//        DeferredEntities.clear();
//    }
//}
//
//void EntityManager::Update(float deltaTime)
//{
//    // TODO: Cleanup at the end.
//    CleanupInactiveEntities();
//}
//
//void EntityManager::CleanupInactiveEntities()
//{
//    std::erase_if(
//        Entities,
//        [](std::unique_ptr<Entity>& e) {
//            return !e->bIsActive;
//        });
//}