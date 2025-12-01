#include "RenderingSystem.h"
#include "../ecs/ECS.h"
#include "../ecs/World.h"
#include "../ecs/components/Transform.h"
#include "../ecs/components/StaticMeshComponent.h"
#include "../renderer/Shader.h"
#include "../renderer/Camera.h"
#include "../renderer/Material.h"
#include "../core/model/StaticMesh.h"
#include "TransformSystem.h"

#include <unordered_map>
#include <vector>

void RenderingSystem::renderEntities(World& world, Shader& shader, Camera& camera, Material& defaultMaterial) {
    shader.use();

    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = camera.getProjectionMatrix();

    shader.setMat4("view", view);
    shader.setMat4("projection", projection);

    std::unordered_map<Material*, std::vector<EntityID>> materialBatches;

    for (EntityID eid : world.registry.getEntitiesWith<StaticMeshComponent>()) {
        if (!world.registry.hasComponent<Transform>(eid)) continue;

        auto& staticMeshComponent = world.registry.getComponent<StaticMeshComponent>(eid);
        if (!staticMeshComponent.bIsVisible) continue;

        Material* mat = staticMeshComponent.material ? staticMeshComponent.material.get() : &defaultMaterial;
        materialBatches[mat].push_back(eid);
    }

    for (auto& [material, entities] : materialBatches) {
        material->bind(shader);

        for (EntityID eid : entities) {
            auto& transform = world.registry.getComponent<Transform>(eid);
            glm::mat4 model = TransformSystem::getTransformMatrix(transform);
            shader.setMat4("model", model);

            auto& staticMeshComponent = world.registry.getComponent<StaticMeshComponent>(eid);
            staticMeshComponent.Mesh->bind();
            staticMeshComponent.Mesh->draw();
        }
    }

    glBindVertexArray(0);
}
