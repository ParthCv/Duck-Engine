#pragma once

class World;
class Entity;

/**
 * Game-specific system that handles duck death visual effects.
 * Separates game-specific death behavior from generic engine lifecycle logic.
 */
class DuckDeathSystem {
public:

    // Updates all dead ducks, applying visual transformations. This runs each frame to catch newly-killed ducks and apply the turkey model.
    void update(World& world, float deltaTime);

    // Handles the visual transformation when a duck dies. Changes the duck model to a turkey and updates scale.
    static void handleDuckDeath(Entity& duck);
};
