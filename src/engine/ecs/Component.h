#pragma once
#include "glad/glad.h"
#include "glm/fwd.hpp"
#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "../ecs/Entity.h"

struct Velocity
{
    glm::vec3 Direction{};
    float Speed{};

    Velocity(glm::vec3 InDirection, float InSpeed) :
        Direction(InDirection),
        Speed(InSpeed)
    {

    }
};

struct Transform {
    // TODO: fix rotation input so that we dont hv to use really small values
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

struct StaticMeshComponent
{
    // TODO: think about storing the material of each entity
    StaticMeshComponent(Entity& InEntity) :
        OwningEntity(&InEntity),
        StaticMeshTransform(Transform{}),
        VAO(0),
        VBO(0)
    {
        StaticMeshTransform = InEntity.getComponent<Transform>();
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

    bool bTicks = true;

    // Material* Material;
    // ..

    glm::mat4 ModelMatrix{};

    void initTransform()
    {
        ModelMatrix = getTransformMatrix();
    }

    glm::mat4 getTransformMatrix()
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, StaticMeshTransform.position + OwningEntity->getComponent<Transform>().position);
        model = glm::rotate(model, StaticMeshTransform.rotation.x + OwningEntity->getComponent<Transform>().rotation.x, glm::vec3(1,0,0));
        model = glm::rotate(model, StaticMeshTransform.rotation.y + OwningEntity->getComponent<Transform>().rotation.y, glm::vec3(0,1,0));
        model = glm::rotate(model, StaticMeshTransform.rotation.z + OwningEntity->getComponent<Transform>().rotation.z, glm::vec3(0,0,1));
        model = glm::scale(model, StaticMeshTransform.scale + OwningEntity->getComponent<Transform>().scale);

        return model;
    }

    void getTransformMatrix(glm::mat4 InTransform)
    {
        ModelMatrix = InTransform;
    }

    void setPosition(glm::vec3 InPosition)
    {
        StaticMeshTransform.position = InPosition;
    }

    void update(float deltaTime)
    {
        if (!bTicks) {
            return;
        }

        // TODO: Implement all update logic below.
    }
};