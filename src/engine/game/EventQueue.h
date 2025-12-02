//
// Created by Nathan on 2025-11-30.
//
#pragma once
#include <vector>

// struct DuckShotEvent {
//     int duck; float x, y;
// };
// struct DuckEscapedEvent {
//     int duck;
// };
// struct ResetDucksEvent { };
// struct BulletFiredEvent {
//     int remainingBullets;
// };
// struct ResetBulletsEvent { };
// struct LevelCompleteEvent {
//     int nextLevel;
// };
// struct StartNextRoundEvent { };
// struct GameOverEvent {
//     bool won;
// };
// struct StartGameEvent { };
// struct UpdateScoreEvent {
//     int newScore;
// };

struct DuckShotEvent {
    int duckId;
    int pointsAwarded;
};

struct DuckEscapedEvent {
    int duckId;
};

struct BulletFiredEvent {
    int remainingBullets;
};

struct RoundStartEvent {
    int round;
    int totalDucks;
};

struct GameOverEvent {
    bool won;
    int finalScore;
    int roundReached;
};

struct StartGameEvent { };

class EventQueue {
    template<typename T>
    static std::vector<T>& getQueue() {
        static std::vector<T> queue;
        return queue;
    }

public:
    template<typename T>
    void emit(const T& event) {
        getQueue<T>().push_back(event);
    }

    template<typename T>
    const std::vector<T>& get() const {
        return getQueue<T>();
    }

    template<typename T>
    bool hasEvents() const {
        return !getQueue<T>().empty();
    }

    void clear() {
        getQueue<DuckShotEvent>().clear();
        getQueue<DuckEscapedEvent>().clear();
        getQueue<BulletFiredEvent>().clear();
        getQueue<RoundStartEvent>().clear();
        getQueue<GameOverEvent>().clear();
    }
};