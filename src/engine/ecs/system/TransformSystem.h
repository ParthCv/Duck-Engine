#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

struct Transform;

class TransformSystem {
public:
    static void SetTransform(Transform& transform, const glm::vec3& position);
    static void AddTransform(Transform& transform, const glm::vec3& translation);
    static void AddLocalTransform(Transform& transform, const glm::vec3& translation);
    static void SetPosition(Transform& transform, const glm::vec3& position);
    static void SetRotation(Transform& transform, const glm::quat& rotation);
    static void LocalRotate(Transform& transform, float angle, const glm::vec3& axis);
    static void WorldRotate(Transform& transform, float angle, const glm::vec3& axis);
    static glm::mat4 getTransformMatrix(const Transform& transform);
};
