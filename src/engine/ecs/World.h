//
// Created by super on 2025-11-03.
//

#ifndef DUCKENGINE_WORLD_H
#define DUCKENGINE_WORLD_H
#include <memory>
#include <vector>

#include "Entity.h"
#include "../system/EntityManager.h"
#include "glad/glad.h"
#include "../core/TestGame.h"

class Shader;
class Camera;

class World
{
public:
    // TODO: Remove later: Temp Variables
    Camera* camera;
    Shader* basicShader;
    EntityManager EntityManager;

    World();

    void Update(float deltaTime);

    void Render();

    void BeginPlay();

    /*
     * Garbage collecting deactivated entities.
     */
    void CleanUp();

    // TODO: Temporary testing function
    void CreateCube(GLuint& InVAO, GLuint& InVBO);

    TestGame* game = nullptr;
};


#endif //DUCKENGINE_WORLD_H