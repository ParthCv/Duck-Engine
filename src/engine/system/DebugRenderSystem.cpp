#include "DebugRenderSystem.h"

#include "EntityManager.h"
#include "../ecs/Entity.h"
#include "../ecs/Component.h"
#include "../debug/DebugRenderer.h"

void DebugRenderSystem::Init() {
    DebugRenderer::getInstance().init();
}

void DebugRenderSystem::Render(EntityManager& entityManager, Shader& debugShader) {
    DrawColliders(entityManager, debugShader);
    DrawRaycasts(entityManager, debugShader);
}

void DebugRenderSystem::DrawColliders(EntityManager& entityManager, Shader& debugShader) {
    auto entities = entityManager.GetEntitiesWith<Transform, BoxCollider, DebugDrawable>();

    for (auto* entity : entities) {
        if (!entity->getIsActive()) continue;

        auto& transform = entity->getComponent<Transform>();
        auto& collider = entity->getComponent<BoxCollider>();
        auto& drawable = entity->getComponent<DebugDrawable>();

        if (!drawable.drawCollider) continue;
        DebugRenderer::getInstance().drawBox(
            debugShader,
            transform.position + collider.center,
            // arbitrary value to make the hitbox larger
            collider.size * 1.1f,
            drawable.colliderColor
        );
    }
}

void DebugRenderSystem::DrawRaycasts(EntityManager& entityManager, Shader& debugShader) {
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

void DebugRenderSystem::Cleanup() {
    DebugRenderer::getInstance().cleanup();
}