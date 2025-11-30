#include "DebugRenderSystem.h"

#include "EntityManager.h"
#include "../ecs/Entity.h"
#include "../ecs/Component.h"
#include "../debug/DebugRenderer.h"

void DebugRenderSystem::init() {
    DebugRenderer::getInstance().init();
}

void DebugRenderSystem::render(EntityManager& entityManager, Shader& debugShader) {
    drawColliders(entityManager, debugShader);
    drawRaycasts(entityManager, debugShader);
}

void DebugRenderSystem::drawColliders(EntityManager& entityManager, Shader& debugShader) {
    auto entities = entityManager.GetEntitiesWith<Transform, BoxCollider, DebugDrawable>();

    for (auto* entity : entities) {
        if (!entity->getIsActive()) continue;

        auto& transform = entity->getComponent<Transform>();
        auto& collider = entity->getComponent<BoxCollider>();
        auto& drawable = entity->getComponent<DebugDrawable>();

        if (!drawable.drawCollider) continue;

        // Apply transform scale to match actual collision size
        glm::vec3 scaledSize = collider.size * transform.scale;

        DebugRenderer::getInstance().drawBox(
            debugShader,
            transform.position + collider.center,
            scaledSize,
            drawable.colliderColor
        );
    }
}

void DebugRenderSystem::drawRaycasts(EntityManager& entityManager, Shader& debugShader) {
    auto entities = entityManager.GetEntitiesWith<Transform, RaycastSource>();

    for (auto* entity : entities) {
        if (!entity->getIsActive()) continue;

        auto& transform = entity->getComponent<Transform>();
        auto& raycastSource = entity->getComponent<RaycastSource>();

        if (!raycastSource.drawRay) continue;

        glm::vec3 start = transform.position;
        glm::vec3 end = raycastSource.lastHit
            ? raycastSource.lastHitPoint
            : start + raycastSource.direction * raycastSource.maxDistance;
        DebugRenderer::getInstance().drawLine(debugShader, start, end, raycastSource.lastHit);
    }
}

void DebugRenderSystem::cleanup() {
    DebugRenderer::getInstance().cleanup();
}