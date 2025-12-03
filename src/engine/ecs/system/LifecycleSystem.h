#pragma once

class World;
class Entity;

class LifecycleSystem {
public:

    void update(World& world, float deltaTime);

    static void killEntity(Entity& entity);

};
