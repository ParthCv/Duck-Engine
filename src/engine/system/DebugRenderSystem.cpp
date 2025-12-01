#include "DebugRenderSystem.h"

#include "EntityManager.h"
#include "../ecs/ECS.h"
#include "../ecs/World.h"
#include "../ecs/Component.h"
#include "../debug/DebugRenderer.h"

void DebugRenderSystem::init() {
    DebugRenderer::getInstance().init();
}

void DebugRenderSystem::render(World& world, Shader& debugShader) {
    drawColliders(world, debugShader);
    drawRaycasts(world, debugShader);
}

void DebugRenderSystem::drawColliders(World& world, Shader& debugShader) {
    for (EntityID eid : world.registry.getEntitiesWith<DebugDrawable>()) {
        if (!world.registry.hasComponent<Transform>(eid)
                || !world.registry.hasComponent<BoxCollider>(eid)) continue;

        // TODO
        //if (!entity->getIsActive()) continue;

        auto& transform = world.registry.getComponent<Transform>(eid);
        auto& collider = world.registry.getComponent<BoxCollider>(eid);
        auto& drawable = world.registry.getComponent<DebugDrawable>(eid);

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

void DebugRenderSystem::drawRaycasts(World& world, Shader& debugShader) {
    for (EntityID eid : world.registry.getEntitiesWith<RaycastSource>()) {
        // TODO
        // if (!entity->getIsActive()) continue;

        auto& transform = world.registry.getComponent<Transform>(eid);
        auto& raycastSource = world.registry.getComponent<RaycastSource>(eid);

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