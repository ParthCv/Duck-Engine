#pragma once

class World;
class Camera;

class GunSystem {
public:
    // Update all guns in the world
    static void update(World& world, Camera& camera, float deltaTime);

    // Apply recoil to a specific gun (call this when shooting)
    static void applyRecoil(class Entity& gunEntity);
};
