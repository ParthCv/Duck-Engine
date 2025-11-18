//
// Created by super on 2025-11-17.
//

#ifndef DUCKENGINE_ENTITYMANAGER_H
#define DUCKENGINE_ENTITYMANAGER_H
#include <memory>
#include <vector>


class DuckEntity;
class World;
class Entity;

class EntityManager
{
public:
    EntityManager();

    void BeginPlay();

    std::vector<std::unique_ptr<Entity>>& GetEntities();

    Entity& CreateEntity(World& InWorld);

    DuckEntity& CreateDuckEntity(World& InWorld);

    Entity& CreateDeferredEntity(World& InWorld);

    void SynchronizeEntities();

    void Update(float deltaTime);

    void CleanupInactiveEntities();

private:
    std::vector<std::unique_ptr<Entity>> Entities;

    std::vector<std::unique_ptr<Entity>> DeferredEntities;
};


#endif //DUCKENGINE_ENTITYMANAGER_H