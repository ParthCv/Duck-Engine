//
// Created by Jeff on 2025-11-18.
//

#include "DebugRenderSystem.h"

#include <iostream>

#include "EntityManager.h"
#include "../ecs/Entity.h"
#include "../ecs/Component.h"

void DebugRenderSystem::Init() {
    DebugRenderer::getInstance().init();
}

void DebugRenderSystem::Render(EntityManager& entityManager, Shader& debugShader) {
    std::cout << "Calling Debug Render" << std::endl;
    DrawColliders(entityManager, debugShader);
    DrawRaycasts(entityManager, debugShader);
}

void DebugRenderSystem::DrawColliders(EntityManager& entityManager, Shader& debugShader) {
    auto entities = entityManager.GetEntitiesWith<Transform, BoxCollider, DebugDrawable>();

    for (auto* entity : entities) {
        if (!entity->GetIsActive()) continue;

        auto& transform = entity->GetComponent<Transform>();
        auto& collider = entity->GetComponent<BoxCollider>();
        auto& drawable = entity->GetComponent<DebugDrawable>();

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
        if (!entity->GetIsActive()) continue;

        auto& transform = entity->GetComponent<Transform>();
        auto& raycastSource = entity->GetComponent<RaycastSource>();

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