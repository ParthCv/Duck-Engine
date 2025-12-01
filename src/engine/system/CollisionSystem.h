#pragma once
#include "../physics/RaycastUtils.h"
#include <vector>
#include <cfloat>
#include "../ecs/ECS.h"
#include "EntityManager.h"

class World;

class CollisionSystem {
public:
    struct RaycastResult {
        bool hit = false;
        EntityID hitEntity = -1;
        Physics::RaycastHit hitInfo;
    };

    RaycastResult Raycast(World& world,
                         const glm::vec3& origin,
                         const glm::vec3& direction,
                         float maxDistance = FLT_MAX);

    RaycastResult RaycastFromEntity(World& world, EntityID eid);

    std::vector<EntityID> GetEntitiesInBox(World& world,
                                          const glm::vec3& min,
                                          const glm::vec3& max);
};
