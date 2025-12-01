#include "GunSystem.h"
#include "../ecs/World.h"
#include "../ecs/Entity.h"
#include "../ecs/components/Transform.h"
#include "../ecs/components/GunComponent.h"
#include "../renderer/Camera.h"
#include "glm/gtc/quaternion.hpp"

void GunSystem::update(World& world, Camera& camera, float deltaTime) {
    // Get all entities with Gun and Transform components
    auto gunEntities = world.EntityManager.GetEntitiesWith<GunComponent, Transform>();

    for (auto* entity : gunEntities) {
        auto& gun = entity->getComponent<GunComponent>();
        auto& transform = entity->getComponent<Transform>();

        // --- RECOIL RECOVERY ---
        // Lerp recoil values back to 0 over time
        gun.recoilOffset = glm::mix(gun.recoilOffset, 0.0f, deltaTime * gun.recoilRecoverySpeed);
        gun.recoilPitch = glm::mix(gun.recoilPitch, 0.0f, deltaTime * gun.recoilRecoverySpeed);

        // --- POSITION UPDATE ---
        // Apply camera offset + recoil kickback
        glm::vec3 kickback = -camera.front * gun.recoilOffset;
        glm::vec3 offset = (camera.right * gun.cameraOffset.x)
                         + (camera.up * gun.cameraOffset.y)
                         + (camera.front * gun.cameraOffset.z);

        transform.position = camera.position + offset + kickback;

        // --- ROTATION UPDATE ---
        // Lock to camera orientation + recoil pitch
        glm::mat3 camRotation(camera.right, camera.up, -camera.front);
        glm::quat orientation = glm::quat_cast(camRotation);

        // Apply muzzle climb (recoil rotation)
        glm::quat recoilRot = glm::angleAxis(gun.recoilPitch, camera.right);
        orientation = recoilRot * orientation;

        // Apply base yaw offset (180° to face forward in FPS)
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
