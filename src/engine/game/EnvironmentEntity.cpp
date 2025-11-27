#include "EnvironmentEntity.h"
#include "../ecs/World.h"
#include "../ecs/Component.h"

EnvironmentEntity::EnvironmentEntity(World &world, glm::vec3 &position, const std::string &modelName)
    : Entity(world) {
    auto& transform = addComponent<Transform>(position, glm::vec3(0.0f, 0.0f, 0.0f),
                                              glm::vec3(1.f,1.f,1.f));
    auto& staticMeshComponent = addComponent<StaticMeshComponent>(*this);
    staticMeshComponent.loadMesh(modelName);
}
