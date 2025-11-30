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
        if (!entity->getIsActive()) continue;

        // Skip dead entities (they shouldn't block raycasts)
        if (entity->hasComponent<HealthComponent>()) {
            auto& health = entity->getComponent<HealthComponent>();
            if (health.isDead) continue;
        }

        auto& transform = entity->getComponent<Transform>();
        auto& collider = entity->getComponent<BoxCollider>();

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
    if (!entity.hasComponent<Transform>() ||
        !entity.hasComponent<RaycastSource>()) {
        return {};
    }

    auto& transform = entity.getComponent<Transform>();
    auto& raycastSource = entity.getComponent<RaycastSource>();

    auto result = Raycast(
        entityManager,
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
        raycastSource.hitEntity = nullptr; // Clear it on miss
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
        if (!entity->getIsActive()) continue;

        // Skip dead entities (they shouldn't be included in area queries)
        if (entity->hasComponent<HealthComponent>()) {
            auto& health = entity->getComponent<HealthComponent>();
            if (health.isDead) continue;
        }

        auto& transform = entity->getComponent<Transform>();
        auto& collider = entity->getComponent<BoxCollider>();

        glm::vec3 scaledSize = collider.size * transform.scale;
        glm::vec3 halfSize = scaledSize * 0.5f;
        glm::vec3 worldCenter = transform.position + collider.center;
        glm::vec3 aabbMin = worldCenter - halfSize;
        glm::vec3 aabbMax = worldCenter + halfSize;

        if (aabbMin.x <= max.x && aabbMax.x >= min.x &&
            aabbMin.y <= max.y && aabbMax.y >= min.y &&
            aabbMin.z <= max.z && aabbMax.z >= min.z) {
            results.push_back(entity);
        }
    }

    return results;
}