//
// Created by super on 2025-11-17.
//

#ifndef DUCKENGINE_ENTITYMANAGER_H
#define DUCKENGINE_ENTITYMANAGER_H
#include <memory>
#include <vector>
#include "../ecs/Entity.h"


class World;

class EntityManager
{
public:
    EntityManager();

    void BeginPlay();

    std::vector<std::unique_ptr<Entity>>& GetEntities();

    Entity& CreateEntity(World& InWorld);

    Entity& CreateDeferredEntity(World& InWorld);

    void SynchronizeEntities();

    void Update();

    void CleanupInactiveEntities();

    template<typename... Components>
std::vector<Entity*> GetEntitiesWith() {
        std::vector<Entity*> result;

        for (auto& entity : Entities) {
            if (!entity->GetIsActive()) continue;

            // Check if entity has ALL required components using fold expression
            if ((entity->HasComponent<Components>() && ...)) {
                result.push_back(entity.get());  // .get() extracts raw pointer from unique_ptr
            }
        }

        return result;
    }

private:
    std::vector<std::unique_ptr<Entity>> Entities;

    std::vector<std::unique_ptr<Entity>> DeferredEntities;
};


#endif //DUCKENGINE_ENTITYMANAGER_H