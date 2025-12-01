#include "RenderingSystem.h"
#include "../src/engine/ecs/World.h"
#include "../src/engine/ecs/Entity.h"
#include "../src/engine/ecs/components/Transform.h"
#include "../src/engine/ecs/components/StaticMeshComponent.h"
#include "../src/engine/renderer/Shader.h"
#include "../src/engine/renderer/Camera.h"
#include "../src/engine/renderer/Material.h"
#include "../src/engine/core/model/StaticMesh.h"
#include "TransformSystem.h"

#include <unordered_map>
#include <vector>

void RenderingSystem::renderEntities(World& world, Shader& shader, Camera& camera, Material& defaultMaterial) {
    shader.use();

    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = camera.getProjectionMatrix();

    shader.setMat4("view", view);
    shader.setMat4("projection", projection);

    std::unordered_map<Material*, std::vector<Entity*>> materialBatches;

    for (auto& entity : world.EntityManager.GetEntities()) {
        if (entity == nullptr) continue;
        if (entity->hasComponent<StaticMeshComponent>() && entity->hasComponent<Transform>()) {
            auto& staticMeshComponent = entity->getComponent<StaticMeshComponent>();
            if (!staticMeshComponent.bIsVisible) continue;

            Material* mat = staticMeshComponent.material ? staticMeshComponent.material.get() : &defaultMaterial;
            materialBatches[mat].push_back(entity.get());
        }
    }

    for (auto& [material, entities] : materialBatches) {
        material->bind(shader);

        for (auto* entity : entities) {
            auto& transform = entity->getComponent<Transform>();
            glm::mat4 model = TransformSystem::getTransformMatrix(transform);
            shader.setMat4("model", model);

            auto& staticMeshComponent = entity->getComponent<StaticMeshComponent>();
            staticMeshComponent.Mesh->bind();
            staticMeshComponent.Mesh->draw();
        }
    }

    glBindVertexArray(0);
}
