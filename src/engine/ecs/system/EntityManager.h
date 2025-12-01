#pragma once
#include <memory>
#include <vector>

#include "../src/engine/ecs/Entity.h"

class EntityManager {

public:
    EntityManager();

    void BeginPlay();

    std::vector<std::unique_ptr<Entity>>& GetEntities();

    Entity& CreateEntity(World& InWorld);

    Entity& CreateDeferredEntity(World& InWorld);

    void SynchronizeEntities();

    void Update(float deltaTime);

    void CleanupInactiveEntities();

    template<typename... Components>
    std::vector<Entity*> GetEntitiesWith();

    template <typename T, typename... Args>
    T& CreateEntityOfType(World& InWorld, Args&&... ConstructorArgs);

private:
    std::vector<std::unique_ptr<Entity>> Entities;

    std::vector<std::unique_ptr<Entity>> DeferredEntities;
};

template<typename... Components>
std::vector<Entity*> EntityManager::GetEntitiesWith() {
    std::vector<Entity*> result;

    for (auto& entity : Entities) {
        if (!entity) continue;
        if (!entity->getIsActive()) continue;

        if ((entity->template hasComponent<Components>() && ...)) {
            result.push_back(entity.get());
        }
    }

    return result;
}

template <typename T, typename... Args>
T& EntityManager::CreateEntityOfType(World& InWorld, Args&&... ConstructorArgs)
{
    static_assert(std::is_base_of<Entity, T>::value, "T must derive from Entity class");
    std::unique_ptr<T> NewEntity = std::make_unique<T>(InWorld, std::forward<Args>(ConstructorArgs)...);
    T* EntityPtr = NewEntity.get(); // must cache this before calling std::move() if we want to return it
    Entities.emplace_back(std::move(NewEntity)); // unique_ptr<T> automatically converts to unique_ptr<Entity>
    return *EntityPtr;
}