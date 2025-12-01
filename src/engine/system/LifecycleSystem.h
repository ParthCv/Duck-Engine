#pragma once

#include "../ecs/ECS.h"

class World;
class Entity;

class LifecycleSystem {
public:
    void update(World& world, float deltaTime);
    static void killDuck(World& world, EntityID eid);
};
