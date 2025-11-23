#pragma once

#include "../renderer/Shader.h"

class EntityManager;
class Shader;

class DebugRenderSystem {

public:
    static void init();
    void render(EntityManager& entityManager, Shader& debugShader);
    static void drawColliders(EntityManager& entityManager, Shader& debugShader);
    static void drawRaycasts(EntityManager& entityManager, Shader& debugShader);
    static void cleanup();
};