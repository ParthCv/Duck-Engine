#pragma once
#include "../ecs/Entity.h"
#include "glm/vec3.hpp"
#include <string>

class EnvironmentEntity : public Entity {
public:
    EnvironmentEntity(World& world, glm::vec3& position, const std::string& modelName);
};
