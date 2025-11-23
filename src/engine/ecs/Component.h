#pragma once
#include "glad/glad.h"
#include "glm/fwd.hpp"
#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "../ecs/Entity.h"
#include "glm/detail/type_quat.hpp"
#include "glm/gtc/quaternion.hpp"

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

// Collision Components
struct BoxCollider {
    glm::vec3 center{0.0f};
    glm::vec3 size{1.0f};
    bool isTrigger = false;

    glm::vec3 GetMin(const Transform& transform) const {
        return transform.position + center - size;
    }

    glm::vec3 GetMax(const Transform& transform) const {
        return transform.position + center + size;
    }
};

struct DebugDrawable {
    bool drawCollider = true;
    glm::vec3 colliderColor{1.0f, 0.0f, 0.0f}; // Red
};

struct RaycastSource {
    glm::vec3 direction{0.0f, 0.0f, -1.0f};
    float maxDistance = 100.0f;
    bool drawRay = true;

    bool lastHit = false;
    glm::vec3 lastHitPoint{};
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
        // StaticMeshTransform = InEntity.getComponent<Transform>();
    }

    // StaticMeshComponent(Entity& InEntity, Transform InTransform) :
    //     OwningEntity(&InEntity),
    //     StaticMeshTransform(InTransform),
    //     VAO(0),
    //     VBO(0)
    // {
    // }

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
        Transform& parent = OwningEntity->getComponent<Transform>();
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