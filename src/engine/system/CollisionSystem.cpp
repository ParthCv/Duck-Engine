#include "CollisionSystem.h"
#include "../ecs/Component.h"
#include "../physics/RaycastUtils.h"
#include "../ecs/World.h"

CollisionSystem::RaycastResult CollisionSystem::Raycast(
    World& world,
    const glm::vec3& origin,
    const glm::vec3& direction,
    float maxDistance)
{
    RaycastResult result;
    float closestDistance = maxDistance;

    for (EntityID eid : world.registry.getEntitiesWith<BoxCollider>()) {
        // TODO
        // if (!entity->getIsActive()) continue;

        // Skip dead entities (they shouldn't block raycasts)
        if (world.registry.hasComponent<HealthComponent>(eid)) {
            auto& health = world.registry.getComponent<HealthComponent>(eid);
            if (health.isDead) continue;
        }

        auto& transform = world.registry.getComponent<Transform>(eid);
        auto& collider = world.registry.getComponent<BoxCollider>(eid);

        glm::vec3 scaledSize = collider.size * transform.scale;
        glm::vec3 halfSize = scaledSize * 0.5f;
        glm::vec3 worldCenter = transform.position + collider.center;
        glm::vec3 aabbMin = worldCenter - halfSize;
        glm::vec3 aabbMax = worldCenter + halfSize;

        Physics::RaycastHit hit = Physics::raycastAABB(
            origin, direction, aabbMin, aabbMax
        );

        if (hit.hit && hit.distance < closestDistance) {
            result.hit = true;
            result.hitEntity = eid;
            result.hitInfo = hit;
            closestDistance = hit.distance;
        }
    }

    return result;
}

CollisionSystem::RaycastResult CollisionSystem::RaycastFromEntity(World& world, EntityID eid)
{
    if (!world.registry.hasComponent<Transform>(eid) ||
        !world.registry.hasComponent<RaycastSource>(eid)) {
        return {};
    }

    auto& transform = world.registry.getComponent<Transform>(eid);
    auto& raycastSource = world.registry.getComponent<RaycastSource>(eid);

    auto result = Raycast(
        world,
        transform.position,
        raycastSource.direction,
        raycastSource.maxDistance
    );

    // ECS Pattern: Update the component data with the result
    raycastSource.lastHit = result.hit;

    if (result.hit) {
        raycastSource.lastHitPoint = result.hitInfo.point;
        raycastSource.hitEntity = result.hitEntity; // Store the Duck/Entity hit
    } else {
        raycastSource.hitEntity = -1; // Clear it on miss
    }

    return result;
}

std::vector<EntityID> CollisionSystem::GetEntitiesInBox(
    World& world,
    const glm::vec3& min,
    const glm::vec3& max)
{
    std::vector<EntityID> results;

    for (EntityID eid : world.registry.getEntitiesWith<BoxCollider>()) {
        // TODO
        //if (!entity->getIsActive()) continue;

        // Skip dead entities (they shouldn't be included in area queries)
        if (world.registry.hasComponent<HealthComponent>(eid)) {
            auto& health = world.registry.getComponent<HealthComponent>(eid);
            if (health.isDead) continue;
        }

        auto& transform = world.registry.getComponent<Transform>(eid);
        auto& collider = world.registry.getComponent<BoxCollider>(eid);

        glm::vec3 scaledSize = collider.size * transform.scale;
        glm::vec3 halfSize = scaledSize * 0.5f;
        glm::vec3 worldCenter = transform.position + collider.center;
        glm::vec3 aabbMin = worldCenter - halfSize;
        glm::vec3 aabbMax = worldCenter + halfSize;

        if (aabbMin.x <= max.x && aabbMax.x >= min.x &&
            aabbMin.y <= max.y && aabbMax.y >= min.y &&
            aabbMin.z <= max.z && aabbMax.z >= min.z) {
            results.push_back(eid);
        }
    }

    return results;
}