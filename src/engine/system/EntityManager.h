#pragma once
#include <memory>
#include <vector>

#include "glm/vec3.hpp"

class World;
class Entity;
class DuckEntity;

class EntityManager {

public:
    EntityManager();

    void BeginPlay();

    std::vector<std::unique_ptr<Entity>>& GetEntities();

    Entity& CreateEntity(World& InWorld);

    DuckEntity& CreateDuckEntity(World& InWorld);

    DuckEntity& CreateDuckEntity(World& InWorld, glm::vec3& InPosition);

    Entity& CreateDeferredEntity(World& InWorld);

    void SynchronizeEntities();

    void Update(float deltaTime);

    void CleanupInactiveEntities();

    template<typename... Components>
    std::vector<Entity*> GetEntitiesWith();

private:
    std::vector<std::unique_ptr<Entity>> Entities;

    std::vector<std::unique_ptr<Entity>> DeferredEntities;
};

#include "../ecs/Entity.h"

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