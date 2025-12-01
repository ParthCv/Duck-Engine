#pragma once

#include "../renderer/Shader.h"

class EntityManager;
class Shader;
class World;

class DebugRenderSystem {

public:
    static void init();
    void render(World& world, Shader& debugShader);
    static void drawColliders(World& world, Shader& debugShader);
    static void drawRaycasts(World& world, Shader& debugShader);
    static void cleanup();
};