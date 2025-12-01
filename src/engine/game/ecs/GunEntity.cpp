#include "../src/engine/game/ecs/GunEntity.h"
#include "../../ecs/Component.h"
#include "../../core/managers/ResourceManager.h"
#include "../../ecs/components/StaticMeshComponent.h"
#include "../../ecs/components/Transform.h"
#include "../../ecs/components/GunComponent.h"

GunEntity::GunEntity(World &world, const std::string &modelName) : Entity(world) {
    auto& transform = addComponent<Transform>(glm::vec3(0.0f, 0.0f, 0.0f),
                                              glm::vec3(0.0f, 0.0f, 0.0f),
                                              glm::vec3(1.f,1.f,1.f));
    auto& staticMeshComponent = addComponent<StaticMeshComponent>();
    staticMeshComponent.Mesh = ResourceManager::Get().GetStaticMesh(modelName);
    staticMeshComponent.material = ResourceManager::Get().GetMaterial("gun");

    // Add gun component for FPS behavior (recoil, camera follow)
    addComponent<GunComponent>();
}
