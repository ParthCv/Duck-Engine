#pragma once

class World;
class Entity;

class GameRoundSystem {
public:
    GameRoundSystem() = default;
    
    void update(World& world, float deltaTime);
    
private:
    void checkRoundCompletion(World& world, Entity& gameEntity);
    void startNextRound(Entity& gameEntity, World& world);
    void endGame(Entity& gameEntity, World& world, bool victory);
};
