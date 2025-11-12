//
// Created by super on 2025-11-03.
//

#ifndef DUCKENGINE_COMPONENT_H
#define DUCKENGINE_COMPONENT_H
#include "glad/glad.h"
#include "glm/fwd.hpp"
#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"
#include "glm/ext/matrix_transform.hpp"

struct Velocity
{
    glm::vec3 Direction{};
    float Speed{};
};

struct Transform {

    Transform()
    {
        position = glm::vec3(0.0f);
        rotation = glm::vec3(0.0f);
        scale = glm::vec3(1.0f);
    }

    Transform(glm::vec3 InPosition, glm::vec3 InRotation, glm::vec3 InScale)
    {
        position = InPosition;
        rotation = InRotation;
        scale = InScale;
    }

    glm::vec3 position = glm::vec3(0,0,0);
    glm::vec3 rotation = glm::vec3(0,0,0);
    glm::vec3 scale{1.0f};
    glm::vec3 oldPosition{};

    void SetTransform(glm::vec3 InTransform) {
        position = InTransform;
    }

    void AddTransform(glm::vec3 InTransform) {
        position += InTransform;
    }

    void SetRotation(glm::vec3 InRotation)
    {
        rotation = InRotation;
    }

    void Rotate(glm::vec3 InRotation)
    {
        rotation += InRotation;
    }

    glm::mat4 Rotate(glm::mat4 ModelMatrix, float InAngle, glm::vec3 InAxis) {
        return glm::rotate(ModelMatrix, InAngle, InAxis);
    }

    Transform operator+(const Transform& InTransform)
    {
        return Transform(position + InTransform.position, rotation + InTransform.rotation, scale + InTransform.scale);
    }

};

struct StaticMesh
{
    GLuint VAO;
    GLuint VBO;
};

struct Material
{

};

struct StaticMeshComponent
{
    StaticMeshComponent(Entity& InEntity) :
        OwningEntity(&InEntity),
        StaticMeshTransform(Transform{}),
        VAO(0),
        VBO(0)
    {
        StaticMeshTransform = InEntity.GetComponent<Transform>();
    }

    StaticMeshComponent(Entity& InEntity, Transform InTransform) :
        OwningEntity(&InEntity),
        StaticMeshTransform(InTransform),
        VAO(0),
        VBO(0)
    {
    }

    Entity* OwningEntity;
    Transform StaticMeshTransform;

    GLuint VAO;
    GLuint VBO;

    // Material* Material;
    // ..

    glm::mat4 ModelMatrix{};

    void InitTransform()
    {
        ModelMatrix = GetTransformMatrix();
    }

    glm::mat4 GetTransformMatrix()
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, StaticMeshTransform.position);
        model = glm::rotate(model, StaticMeshTransform.rotation.x, glm::vec3(1,0,0));
        model = glm::rotate(model, StaticMeshTransform.rotation.y, glm::vec3(0,1,0));
        model = glm::rotate(model, StaticMeshTransform.rotation.z, glm::vec3(0,0,1));
        model = glm::scale(model, StaticMeshTransform.scale);

        return model;
    }

    void SetTransformMatrix(glm::mat4 InTransform)
    {
        ModelMatrix = InTransform;
    }

    void SetPosition(glm::vec3 InPosition)
    {
        auto& OwningEntityTransform = OwningEntity->GetComponent<Transform>();
        StaticMeshTransform.position = OwningEntityTransform.position + InPosition;
    }
};

#endif //DUCKENGINE_COMPONENT_H