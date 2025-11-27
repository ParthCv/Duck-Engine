
#ifndef DUCKENGINE_GUNENTITY_H
#define DUCKENGINE_GUNENTITY_H
#include "../ecs/Entity.h"
#include "glm/vec3.hpp"
#include <string>

class GunEntity : public Entity {
public:
    GunEntity(World& world, const std::string& modelName);
};

#endif //DUCKENGINE_GUNENTITY_H
