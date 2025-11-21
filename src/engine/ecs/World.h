//
// Created by super on 2025-11-03.
//

#ifndef DUCKENGINE_WORLD_H
#define DUCKENGINE_WORLD_H

#include "../system/EntityManager.h"
#include "../system/DebugRenderSystem.h"
#include "../system/CollisionSystem.h"

#include "glad/glad.h"


class Shader;
class Camera;
class CollisionSystem;

class World
{
public:
    // TODO: Remove later: Temp Variables
    Camera* camera;
    Shader* basicShader;
    EntityManager EntityManager;
    CollisionSystem* collisionSystem = nullptr;
    DebugRenderSystem debugRenderSystem;

    World();

    void Update(float deltaTime);

    void Render();

    void BeginPlay();

    void TestRandomRaycasting(float deltaTime);

    /*
     * Garbage collecting deactivated entities.
     */
    void CleanUp();

    // TODO: Temporary testing function
    void CreateCube(GLuint& InVAO, GLuint& InVBO);
    void CreateLine(GLuint& InVAO, GLuint& InVBO);

};


#endif //DUCKENGINE_WORLD_H