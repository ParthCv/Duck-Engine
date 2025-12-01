#pragma once

#include "glm/glm.hpp"

class World;
class Entity;

class DuckFactory {
public:
    static Entity* createDuck(World& world, const glm::vec3& position, float speed);
};
