#pragma once
#include "../src/engine/system/EntityManager.h"
#include "../src/engine/renderer/light/LightManager.h"
#include "../src/engine/renderer/Shader.h"
#include "../src/engine/renderer/Camera.h"
#include "glad/glad.h"

class World
{
public:
    // TODO: Remove later: Temp Variables
    Camera* camera;
    Shader* basicShader;
    EntityManager EntityManager;
    LightManager lightManager;

    World();

    void Update(float deltaTime);

    void Render();

    void BeginPlay();

    void addLightsToWorld();

    /*
     * Garbage collecting deactivated entities.
     */
    void CleanUp();

    // TODO: Temporary testing function
    void CreateCube(GLuint& InVAO, GLuint& InVBO);

private:
    std::vector<glm::vec3> duckPos = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(2.5f, 0.0f, 0.0f),
        glm::vec3(-2.5f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 2.5f),
        glm::vec3(0.0f, 0.0f, -2.5f),
        glm::vec3(2.5f, 0.0f, 2.5f),
        glm::vec3(-2.5f, 0.0f, 2.5f),
        glm::vec3(2.5f, 0.0f, -2.5f),
        glm::vec3(-2.5f, 0.0f, -2.5f),
        glm::vec3(0.0f, 2.0f, 0.0f),
    };
};

