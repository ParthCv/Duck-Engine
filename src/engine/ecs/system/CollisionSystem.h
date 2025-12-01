#pragma once
#include "../src/engine/physics/RaycastUtils.h"
#include <vector>
#include <cfloat>

class EntityManager;
class Entity;

class CollisionSystem {
public:
    struct RaycastResult {
        bool hit = false;
        Entity* hitEntity = nullptr;
        Physics::RaycastHit hitInfo;
    };

    RaycastResult Raycast(EntityManager& entityManager,
                         const glm::vec3& origin,
                         const glm::vec3& direction,
                         float maxDistance = FLT_MAX);

    RaycastResult RaycastFromEntity(EntityManager& entityManager, Entity& entity);

    std::vector<Entity*> GetEntitiesInBox(EntityManager& entityManager,
                                          const glm::vec3& min,
                                          const glm::vec3& max);
};
