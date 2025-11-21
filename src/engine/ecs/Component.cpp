//
// Created by Jeff on 2025-11-19.
//


#include "Component.h"
#include "Entity.h"

StaticMeshComponent::StaticMeshComponent(Entity& InEntity) :
    OwningEntity(&InEntity),
    StaticMeshTransform(Transform{}),
    VAO(0),
    VBO(0)
{
    StaticMeshTransform = InEntity.GetComponent<Transform>();
}

StaticMeshComponent::StaticMeshComponent(Entity& InEntity, Transform InTransform) :
    OwningEntity(&InEntity),
    StaticMeshTransform(InTransform),
    VAO(0),
    VBO(0)
{
}

void StaticMeshComponent::SetPosition(glm::vec3 InPosition)
{
    auto& OwningEntityTransform = OwningEntity->GetComponent<Transform>();
    StaticMeshTransform.position = OwningEntityTransform.position + InPosition;
}