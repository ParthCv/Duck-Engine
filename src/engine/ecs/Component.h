#pragma once
#include "glad/glad.h"
#include "glm/fwd.hpp"
#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "../ecs/Entity.h"
#include "glm/detail/type_quat.hpp"
#include "glm/gtc/quaternion.hpp"
#include "../core/model/StaticMesh.h"

struct Velocity
{
    glm::vec3 Direction{};
    float Speed{};

    Velocity(glm::vec3 InDirection, float InSpeed) :
        Direction(InDirection),
        Speed(InSpeed)
    {

    }

    void setVelocity(glm::vec3 InDirection, float InSpeed) {
        Direction = InDirection;
        Speed = InSpeed;
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
    glm::quat rotation = glm::quat();
    glm::vec3 scale{1.0f};
    glm::vec3 oldPosition{};

    void SetTransform(glm::vec3 InTransform) {
        position = InTransform;
    }

    void AddTransform(glm::vec3 InTransform) {
        position += InTransform;
    }

    void AddLocalTransform(glm::vec3 InTransform) {
        position += glm::vec3(getTransformMatrix() * glm::vec4(InTransform, 0.0f));
    }

    void SetRotation(glm::vec3 InRotation)
    {
        rotation = InRotation;
    }

    /*
     * Rotation using radians, about a specific axis.
     */
    // void Rotate(float amount, glm::vec3 axis)
    // {
    //     // rotation += axis * glm::radians(amount);
    //     rotation *= glm::quat(glm::radians(axis * amount));
    // }

    glm::mat4 Rotate(glm::mat4 ModelMatrix, float InAngle, glm::vec3 InAxis) {
        return glm::rotate(ModelMatrix, InAngle, InAxis);
    }

    void LocalRotate(float angle, glm::vec3 axis) {
        glm::vec3 worldAxis = rotation * glm::normalize(axis);
        glm::quat delta = glm::angleAxis(glm::radians(angle), worldAxis);
        rotation = rotation * delta;
    }

    void WorldRotate(float angle, glm::vec3 axis) {
        glm::quat delta = glm::angleAxis(glm::radians(angle), axis);
        rotation = delta * rotation;
    }

    glm::mat4 getTransformMatrix()
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        // model = glm::rotate(model, rotation.x, glm::vec3(1,0,0));
        // model = glm::rotate(model, rotation.y, glm::vec3(0,1,0));
        // model = glm::rotate(model, rotation.z, glm::vec3(0,0,1));
        model *= glm::mat4_cast(rotation);
        model = glm::scale(model, scale);

        return model;
    }

    // Transform operator+(const Transform& InTransform)
    // {
    //     return Transform(position + InTransform.position, rotation + InTransform.rotation, scale + InTransform.scale);
    // }

};

struct StaticMeshComponent
{
    // TODO: think about storing the material of each entity
    explicit StaticMeshComponent(Entity& InEntity)
            : OwningEntity(&InEntity),
            StaticMeshTransform(Transform{}),
            Mesh(std::make_shared<StaticMesh>())
    {
    }

    Entity* OwningEntity;
    Transform StaticMeshTransform;
    glm::mat4 ModelMatrix{};
    std::shared_ptr<StaticMesh> Mesh;
    bool bIsVisible = true;
    bool bTicks = true;

    void loadMesh(const std::string& modelFilePath) {
        ImportedModel model{modelFilePath};
        Mesh->loadFromImportedModel(model);
    }

    void initTransform()
    {
        ModelMatrix = getTransformMatrix();
    }

    glm::mat4 getTransformMatrix()
    {
        auto& parent = OwningEntity->getComponent<Transform>();
        return parent.getTransformMatrix() * StaticMeshTransform.getTransformMatrix();
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