//
// Created by super on 2025-11-03.
//

#ifndef DUCKENGINE_WORLD_H
#define DUCKENGINE_WORLD_H
#include <memory>
#include <vector>

#include "Entity.h"
#include "glad/glad.h"

class Shader;
class Camera;

class World
{
private:
    std::vector<std::unique_ptr<Entity>> Entities;
public:
    // TODO: Remove later: Temp Variables
    Camera* camera;
    Shader* basicShader;

    World();

    void Update(float deltaTime);

    void Render();

    void BeginPlay();

    Entity& CreateEntity(World& InWorld);

    std::vector<std::unique_ptr<Entity>>& GetEntities();

    /*
     * Garbage collecting deactivated entities.
     */
    void CleanUp();

    // TODO: Temporary testing function
    void CreateCube(GLuint& InVAO, GLuint& InVBO);
};


#endif //DUCKENGINE_WORLD_H