#include "GameStateSystem.h"
#include "../../../ecs/World.h"
#include "../../../ecs/Entity.h"
#include "../components/GameRoundComponent.h"
#include "../../../core/managers/AudioManager.h"
#include "../../EventQueue.h"
#include <cmath>

// Static event queue for game events
static EventQueue eventQueue;

Entity* GameStateSystem::getGameStateEntity(World& world) {
    return world.getGameStateEntity();
}


// === Round Management ===

bool GameStateSystem::canSpawnMoreDucks(Entity& gameState) {
    if (!gameState.hasComponent<GameRoundComponent>()) return false;
    auto& round = gameState.getComponent<GameRoundComponent>();
    return round.duckSpawnIndex < round.maxDucksPerRound;
}

bool GameStateSystem::isRoundComplete(Entity& gameState) {
    if (!gameState.hasComponent<GameRoundComponent>()) return false;
    auto& round = gameState.getComponent<GameRoundComponent>();
    return round.duckSpawnIndex >= round.maxDucksPerRound &&
           round.duckResolveIndex >= round.maxDucksPerRound;
}

bool GameStateSystem::isRoundFailed(Entity& gameState) {
    if (!gameState.hasComponent<GameRoundComponent>()) return false;
    auto& round = gameState.getComponent<GameRoundComponent>();
    return isRoundComplete(gameState) && round.ducksEscaped > round.maxDucksEscapedAllowed;
}

float GameStateSystem::getDuckSpeed(Entity& gameState) {
    if (!gameState.hasComponent<GameRoundComponent>()) return 0.5f;
    auto& round = gameState.getComponent<GameRoundComponent>();
    return round.initialDuckSpeed * std::pow(round.duckSpeedMultiplier, round.currentRound - 1);
}

void GameStateSystem::startNextRound(Entity& gameState) {
    if (!gameState.hasComponent<GameRoundComponent>()) return;
    if (!gameState.hasComponent<AmmoComponent>()) return;
    if (!gameState.hasComponent<DuckUIStateComponent>()) return;

    auto& round = gameState.getComponent<GameRoundComponent>();
    auto& ammo = gameState.getComponent<AmmoComponent>();

    round.currentRound++;
    ammo.current = ammo.max;
    round.ducksEscaped = 0;
    round.duckSpawnIndex = 0;
    round.duckResolveIndex = 0;

    resetDuckStates(gameState);

    AudioManager::Get().PlaySound("win", 0.8f);
    eventQueue.emit(RoundStartEvent{round.currentRound, round.maxDucksPerRound});
}

void GameStateSystem::endGameDefeat(Entity& gameState) {
    if (!gameState.hasComponent<ScoreComponent>()) return;
    if (!gameState.hasComponent<GameRoundComponent>()) return;

    auto& score = gameState.getComponent<ScoreComponent>();
    auto& round = gameState.getComponent<GameRoundComponent>();

    eventQueue.emit(GameOverEvent{false, score.totalScore, round.currentRound});
}

void GameStateSystem::endGameVictory(Entity& gameState) {
    if (!gameState.hasComponent<ScoreComponent>()) return;
    if (!gameState.hasComponent<GameRoundComponent>()) return;

    auto& score = gameState.getComponent<ScoreComponent>();
    auto& round = gameState.getComponent<GameRoundComponent>();

    eventQueue.emit(GameOverEvent{true, score.totalScore, round.currentRound});
}


// === Ammo Management ===

bool GameStateSystem::hasAmmo(Entity& gameState) {
    if (!gameState.hasComponent<AmmoComponent>()) return false;
    auto& ammo = gameState.getComponent<AmmoComponent>();
    return ammo.current > 0;
}

void GameStateSystem::consumeAmmo(Entity& gameState) {
    if (!gameState.hasComponent<AmmoComponent>()) return;
    auto& ammo = gameState.getComponent<AmmoComponent>();
    if (ammo.current > 0) {
        ammo.current--;
        eventQueue.emit(BulletFiredEvent{ammo.current});
    }
}

void GameStateSystem::refillAmmo(Entity& gameState) {
    if (!gameState.hasComponent<AmmoComponent>()) return;
    auto& ammo = gameState.getComponent<AmmoComponent>();
    ammo.current = ammo.max;
}


// === Duck Tracking ===

void GameStateSystem::spawnDuck(Entity& gameState) {
    if (!gameState.hasComponent<GameRoundComponent>()) return;
    if (!gameState.hasComponent<DuckUIStateComponent>()) return;

    auto& round = gameState.getComponent<GameRoundComponent>();
    auto& duckUI = gameState.getComponent<DuckUIStateComponent>();

    if (canSpawnMoreDucks(gameState)) {
        duckUI.states[round.duckSpawnIndex] = DuckState::SPAWNED;
        round.duckSpawnIndex++;
    }
}

void GameStateSystem::hitDuck(Entity& gameState) {
    if (!gameState.hasComponent<GameRoundComponent>()) return;
    if (!gameState.hasComponent<ScoreComponent>()) return;
    if (!gameState.hasComponent<DuckUIStateComponent>()) return;

    auto& round = gameState.getComponent<GameRoundComponent>();
    auto& score = gameState.getComponent<ScoreComponent>();
    auto& duckUI = gameState.getComponent<DuckUIStateComponent>();

    if (round.duckResolveIndex < round.maxDucksPerRound) {
        int points = static_cast<int>(score.basePointsPerDuck *
                                     std::pow(score.scoreMultiplier, round.currentRound - 1));
        score.totalScore += points;

        duckUI.states[round.duckResolveIndex] = DuckState::HIT;

        eventQueue.emit(DuckShotEvent{round.duckResolveIndex, points});

        round.duckResolveIndex++;
    }
}

void GameStateSystem::duckEscaped(Entity& gameState) {
    if (!gameState.hasComponent<GameRoundComponent>()) return;
    if (!gameState.hasComponent<DuckUIStateComponent>()) return;

    auto& round = gameState.getComponent<GameRoundComponent>();
    auto& duckUI = gameState.getComponent<DuckUIStateComponent>();

    if (round.duckResolveIndex < round.maxDucksPerRound) {
        round.ducksEscaped++;

        duckUI.states[round.duckResolveIndex] = DuckState::ESCAPED;
        AudioManager::Get().PlaySound("flapping", 1.0f);
        eventQueue.emit(DuckEscapedEvent{round.duckResolveIndex});

        round.duckResolveIndex++;
    }
}


// === Getters ===

int GameStateSystem::getRound(Entity& gameState) {
    if (!gameState.hasComponent<GameRoundComponent>()) return 1;
    return gameState.getComponent<GameRoundComponent>().currentRound;
}

int GameStateSystem::getScore(Entity& gameState) {
    if (!gameState.hasComponent<ScoreComponent>()) return 0;
    return gameState.getComponent<ScoreComponent>().totalScore;
}

int GameStateSystem::getNumOfBullets(Entity& gameState) {
    if (!gameState.hasComponent<AmmoComponent>()) return 0;
    return gameState.getComponent<AmmoComponent>().current;
}

int GameStateSystem::getMaxNumOfBullets(Entity& gameState) {
    if (!gameState.hasComponent<AmmoComponent>()) return 10;
    return gameState.getComponent<AmmoComponent>().max;
}

int GameStateSystem::getMaxNumOfDucks(Entity& gameState) {
    if (!gameState.hasComponent<GameRoundComponent>()) return 10;
    return gameState.getComponent<GameRoundComponent>().maxDucksPerRound;
}

int GameStateSystem::getNumOfDucksEscaped(Entity& gameState) {
    if (!gameState.hasComponent<GameRoundComponent>()) return 0;
    return gameState.getComponent<GameRoundComponent>().ducksEscaped;
}

int GameStateSystem::getDuckSpawnIndex(Entity& gameState) {
    if (!gameState.hasComponent<GameRoundComponent>()) return 0;
    return gameState.getComponent<GameRoundComponent>().duckSpawnIndex;
}

int GameStateSystem::getDuckResolveIndex(Entity& gameState) {
    if (!gameState.hasComponent<GameRoundComponent>()) return 0;
    return gameState.getComponent<GameRoundComponent>().duckResolveIndex;
}


// === Setters ===

void GameStateSystem::setMaxNumOfDucks(Entity& gameState, int num) {
    if (!gameState.hasComponent<GameRoundComponent>()) return;
    auto& round = gameState.getComponent<GameRoundComponent>();
    round.maxDucksPerRound = num;
}

void GameStateSystem::resetGameState(Entity& gameState) {
    if (!gameState.hasComponent<GameRoundComponent>()) return;
    if (!gameState.hasComponent<AmmoComponent>()) return;
    if (!gameState.hasComponent<ScoreComponent>()) return;

    auto& round = gameState.getComponent<GameRoundComponent>();
    auto& ammo = gameState.getComponent<AmmoComponent>();
    auto& score = gameState.getComponent<ScoreComponent>();

    round.currentRound = 1;
    score.totalScore = 0;
    ammo.current = ammo.max;
    round.ducksEscaped = 0;
    round.duckSpawnIndex = 0;
    round.duckResolveIndex = 0;

    resetDuckStates(gameState);
}


// === Duck State UI ===

int GameStateSystem::getDuckStateAtIndex(Entity& gameState, int index) {
    if (!gameState.hasComponent<DuckUIStateComponent>()) return static_cast<int>(DuckState::NOT_SPAWNED);

    auto& duckUI = gameState.getComponent<DuckUIStateComponent>();
    if (index >= 0 && index < DuckUIStateComponent::MAX_DUCKS) {
        return static_cast<int>(duckUI.states[index]);
    }
    return static_cast<int>(DuckState::NOT_SPAWNED);
}

void GameStateSystem::resetDuckStates(Entity& gameState) {
    if (!gameState.hasComponent<DuckUIStateComponent>()) return;

    auto& duckUI = gameState.getComponent<DuckUIStateComponent>();
    for (int i = 0; i < DuckUIStateComponent::MAX_DUCKS; i++) {
        duckUI.states[i] = DuckState::NOT_SPAWNED;
    }
}


// === Event Management ===

// Explicit template instantiations for event types
template<typename T>
const std::vector<T>& GameStateSystem::getEvents() {
    return eventQueue.get<T>();
}

template<typename T>
bool GameStateSystem::hasEvents() {
    return eventQueue.hasEvents<T>();
}

void GameStateSystem::clearEvents() {
    eventQueue.clear();
}


// Explicit instantiations for the event types we use
template const std::vector<DuckShotEvent>& GameStateSystem::getEvents<DuckShotEvent>();
template const std::vector<DuckEscapedEvent>& GameStateSystem::getEvents<DuckEscapedEvent>();
template const std::vector<BulletFiredEvent>& GameStateSystem::getEvents<BulletFiredEvent>();
template const std::vector<RoundStartEvent>& GameStateSystem::getEvents<RoundStartEvent>();
template const std::vector<GameOverEvent>& GameStateSystem::getEvents<GameOverEvent>();

template bool GameStateSystem::hasEvents<DuckShotEvent>();
template bool GameStateSystem::hasEvents<DuckEscapedEvent>();
template bool GameStateSystem::hasEvents<BulletFiredEvent>();
template bool GameStateSystem::hasEvents<RoundStartEvent>();
template bool GameStateSystem::hasEvents<GameOverEvent>();
