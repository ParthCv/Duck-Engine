#pragma once
#include <memory>
#include <vector>

#include "glm/vec3.hpp"
#include "../ecs/Entity.h"
#include "../src/engine/game/DuckEntity.h"

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

private:
    std::vector<std::unique_ptr<Entity>> Entities;

    std::vector<std::unique_ptr<Entity>> DeferredEntities;

    //TODO: store all the renderable entites to pass to the engine to render
};