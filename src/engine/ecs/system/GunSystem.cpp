#include "GunSystem.h"
#include "../src/engine/ecs/World.h"
#include "../src/engine/ecs/Entity.h"
#include "../src/engine/ecs/components/Transform.h"
#include "../src/engine/ecs/components/GunComponent.h"
#include "../src/engine/ecs/components/RaycastSource.h"
#include "../src/engine/ecs/components/HealthComponent.h"
#include "../src/engine/renderer/Camera.h"
#include "glm/gtc/quaternion.hpp"
#include "../../game/ecs/system/GameStateSystem.h"
#include "../../core/managers/AudioManager.h"
#include "../../ecs/system/CollisionSystem.h"

struct HealthComponent;
struct RaycastSource;

void GunSystem::update(World& world, Camera& camera, float deltaTime) {
    // Get all entities with Gun and Transform components
    auto gunEntities = world.EntityManager.GetEntitiesWith<GunComponent, Transform>();

    for (auto* entity : gunEntities) {
        auto& gun = entity->getComponent<GunComponent>();
        auto& transform = entity->getComponent<Transform>();

        // Lerp recoil values back to 0 over time
        gun.recoilOffset = glm::mix(gun.recoilOffset, 0.0f, deltaTime * gun.recoilRecoverySpeed);
        gun.recoilPitch = glm::mix(gun.recoilPitch, 0.0f, deltaTime * gun.recoilRecoverySpeed);

        // Apply camera offset + recoil kickback
        glm::vec3 kickback = -camera.front * gun.recoilOffset;
        glm::vec3 offset = (camera.right * gun.cameraOffset.x)
                         + (camera.up * gun.cameraOffset.y)
                         + (camera.front * gun.cameraOffset.z);

        transform.position = camera.position + offset + kickback;

        // Lock to camera orientation + recoil pitch
        glm::mat3 camRotation(camera.right, camera.up, -camera.front);
        glm::quat orientation = glm::quat_cast(camRotation);

        // Apply muzzle climb (recoil rotation)
        glm::quat recoilRot = glm::angleAxis(gun.recoilPitch, camera.right);
        orientation = recoilRot * orientation;

        // Apply base yaw offset
        orientation = orientation * glm::angleAxis(glm::radians(gun.baseYawOffset), glm::vec3(0, 1, 0));

        transform.rotation = orientation;
    }
}

void GunSystem::applyRecoil(Entity& gunEntity) {
    if (!gunEntity.hasComponent<GunComponent>()) return;

    auto& gun = gunEntity.getComponent<GunComponent>();
    gun.recoilOffset = gun.recoilOffsetAmount;
    gun.recoilPitch = gun.recoilPitchAmount;
}

void GunSystem::fire(World& world, Entity& gunEntity, Entity& sourceEntity, const Camera& camera) {
    // Get game state entity for ammo tracking
    Entity* gameState = world.getGameStateEntity();
    if (!gameState) return;

    if (!GameStateSystem::hasAmmo(*gameState)) {
        AudioManager::Get().PlaySound("no-ammo", 0.5f);
        return;
    }

    GameStateSystem::consumeAmmo(*gameState);
    AudioManager::Get().PlaySound("shoot", 0.5f);

    applyRecoil(gunEntity);

    if (sourceEntity.hasComponent<RaycastSource>()) {
        auto& raySource = sourceEntity.getComponent<RaycastSource>();

        raySource.direction = camera.front;
        raySource.drawRay = true;

        if (world.collisionSystem) {
            auto result = world.collisionSystem->RaycastFromEntity(world.EntityManager, sourceEntity);

            if (result.hit && result.hitEntityID != INVALID_ENTITY_ID) {
                Entity* hitEntity = world.EntityManager.GetEntityByID(result.hitEntityID);
                if (hitEntity && hitEntity->hasComponent<HealthComponent>()) {
                    world.lifecycleSystem.killEntity(*hitEntity);
                }
            }
        }
    }
}
