//
// Created by Jeff on 2025-11-18.
//

#include "CollisionSystem.h"
#include "../ecs/Component.h"
#include "../physics/RaycastUtils.h"
#include "../system/EntityManager.h"

CollisionSystem::RaycastResult CollisionSystem::Raycast(
    EntityManager& entityManager,
    const glm::vec3& origin,
    const glm::vec3& direction,
    float maxDistance)
{
    RaycastResult result;
    float closestDistance = maxDistance;

    auto entities = entityManager.GetEntitiesWith<Transform, BoxCollider>();

    for (auto* entity : entities) {
        if (!entity->GetIsActive()) continue;

        auto& transform = entity->GetComponent<Transform>();
        auto& collider = entity->GetComponent<BoxCollider>();

        glm::vec3 aabbMin = collider.GetMin(transform);
        glm::vec3 aabbMax = collider.GetMax(transform);

        Physics::RaycastHit hit = Physics::raycastAABB(
            origin, direction, aabbMin, aabbMax
        );

        if (hit.hit && hit.distance < closestDistance) {
            result.hit = true;
            result.hitEntity = entity;
            result.hitInfo = hit;
            closestDistance = hit.distance;
        }
    }

    return result;
}

CollisionSystem::RaycastResult CollisionSystem::RaycastFromEntity(
    EntityManager& entityManager,
    Entity& entity)
{
    if (!entity.HasComponent<Transform>() ||
        !entity.HasComponent<RaycastSource>()) {
        return {};
    }

    auto& transform = entity.GetComponent<Transform>();
    auto& raycastSource = entity.GetComponent<RaycastSource>();

    auto result = Raycast(
        entityManager,
        transform.position,
        raycastSource.direction,
        raycastSource.maxDistance
    );

    raycastSource.lastHit = result.hit;
    if (result.hit) {
        raycastSource.lastHitPoint = result.hitInfo.point;
    }

    return result;
}

std::vector<Entity*> CollisionSystem::GetEntitiesInBox(
    EntityManager& entityManager,
    const glm::vec3& min,
    const glm::vec3& max)
{
    std::vector<Entity*> results;
    auto entities = entityManager.GetEntitiesWith<Transform, BoxCollider>();

    for (auto* entity : entities) {
        if (!entity->GetIsActive()) continue;

        auto& transform = entity->GetComponent<Transform>();
        auto& collider = entity->GetComponent<BoxCollider>();

        glm::vec3 aabbMin = collider.GetMin(transform);
        glm::vec3 aabbMax = collider.GetMax(transform);

        if (aabbMin.x <= max.x && aabbMax.x >= min.x &&
            aabbMin.y <= max.y && aabbMax.y >= min.y &&
            aabbMin.z <= max.z && aabbMax.z >= min.z) {
            results.push_back(entity);
        }
    }

    return results;
}