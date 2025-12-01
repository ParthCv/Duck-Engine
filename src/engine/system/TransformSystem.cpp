#include "TransformSystem.h"
#include "../ecs/components/Transform.h"
#include "glm/gtc/matrix_transform.hpp"

void TransformSystem::SetTransform(Transform& transform, const glm::vec3& position) {
    transform.position = position;
}

void TransformSystem::AddTransform(Transform& transform, const glm::vec3& translation) {
    transform.position += translation;
}

void TransformSystem::AddLocalTransform(Transform& transform, const glm::vec3& translation) {
    transform.position += glm::vec3(getTransformMatrix(transform) * glm::vec4(translation, 0.0f));
}

void TransformSystem::SetPosition(Transform& transform, const glm::vec3& position) {
    transform.position = position;
}

void TransformSystem::SetRotation(Transform& transform, const glm::quat& rotation) {
    transform.rotation = rotation;
}

void TransformSystem::LocalRotate(Transform& transform, float angle, const glm::vec3& axis) {
    glm::vec3 worldAxis = transform.rotation * glm::normalize(axis);
    glm::quat delta = glm::angleAxis(glm::radians(angle), worldAxis);
    transform.rotation = transform.rotation * delta;
}

void TransformSystem::WorldRotate(Transform& transform, float angle, const glm::vec3& axis) {
    glm::quat delta = glm::angleAxis(glm::radians(angle), axis);
    transform.rotation = delta * transform.rotation;
}

glm::mat4 TransformSystem::getTransformMatrix(const Transform& transform) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, transform.position);
    model *= glm::mat4_cast(transform.rotation);
    model = glm::scale(model, transform.scale);
    return model;
}
