#pragma once

class World;
class Entity;

/**
 * Generic engine system for handling entity lifecycle (death, falling, cleanup).
 * Game-specific death behavior should be in game-specific systems.
 */
class LifecycleSystem {
public:
    void update(World& world, float deltaTime);

    /**
     * Generic method to kill any entity with a HealthComponent.
     * Sets isDead flag and stops movement. Game-specific systems
     * should handle visual transformations and audio.
     */
    static void killEntity(Entity& entity);
};
