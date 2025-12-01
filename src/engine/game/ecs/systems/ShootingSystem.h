#pragma once

class World;
class Entity;

class ShootingSystem {
public:
    ShootingSystem() = default;
    
    void update(World& world, float deltaTime);
    
private:
    void processShot(World& world, Entity& playerEntity, Entity& gameEntity);
};
