#include "GunEntity.h"
#include "../ecs/Component.h"

GunEntity::GunEntity(World &world, const std::string &modelName) : Entity(world) {
    auto& transform = addComponent<Transform>(glm::vec3(0.0f, 0.0f, 0.0f),
                                              glm::vec3(0.0f, 0.0f, 0.0f),
                                              glm::vec3(1.f,1.f,1.f));
    auto& staticMeshComponent = addComponent<StaticMeshComponent>(*this);
    staticMeshComponent.loadMesh(modelName);
}
