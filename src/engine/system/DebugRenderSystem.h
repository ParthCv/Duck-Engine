//
// Created by Jeff on 2025-11-18.
//
#ifndef DUCKENGINE_DEBUGRENDERSYSTEM_H
#define DUCKENGINE_DEBUGRENDERSYSTEM_H

#include "../debug/DebugRenderer.h"

class EntityManager;
class Shader;

class DebugRenderSystem {

public:

    void Init();
    void Render(EntityManager& entityManager, Shader& debugShader);
    void DrawColliders(EntityManager& entityManager, Shader& debugShader);
    void DrawRaycasts(EntityManager& entityManager, Shader& debugShader);
    void Cleanup();
};

#endif