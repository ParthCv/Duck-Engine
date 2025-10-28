#include "Engine/Core/Engine.h"
#include <iostream>

#define WIDTH 1920
#define HEIGHT 1080

int main() {
    Engine engine;

    if (!engine.initialize(WIDTH, HEIGHT)) {
        std::cerr << "Failed to initialize engine" << std::endl;
        return -1;
    }

    engine.run();
    engine.shutdown();

    return 0;
}