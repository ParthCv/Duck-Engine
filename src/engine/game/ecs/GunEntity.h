#pragma once
#include "../../ecs/Entity.h"
#include <string>

class GunEntity : public Entity {
public:
    GunEntity(World& world, const std::string& modelName);
};
