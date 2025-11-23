//
// Created by Jeff on 2025-11-18.
//
#ifndef DUCKENGINE_DEBUGRENDERSYSTEM_H
#define DUCKENGINE_DEBUGRENDERSYSTEM_H

#include "../renderer/Shader.h"

class EntityManager;
class Shader;

class DebugRenderSystem {

public:
    static void Init();
    void Render(EntityManager& entityManager, Shader& debugShader);
    static void DrawColliders(EntityManager& entityManager, Shader& debugShader);
    static void DrawRaycasts(EntityManager& entityManager, Shader& debugShader);
    static void Cleanup();
};

#endif