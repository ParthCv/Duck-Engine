#pragma once

#include "glm/glm.hpp"
#include "../ecs/ECS.h"

class World;
class Entity;

class DuckFactory {
public:
    static EntityID createDuck(World& world, const glm::vec3& position, float speed);
};
