
#ifndef DUCKENGINE_STATICMESH_H
#define DUCKENGINE_STATICMESH_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "ImportedModel.h"

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

class StaticMesh
{
    GLuint VAO;
    std::vector<GLuint> VBOs;
    GLuint EBO;
    GLsizei indexCount;
    glm::vec3 minBounds;
    glm::vec3 maxBounds;
    glm::vec3 size;
    glm::vec3 center;

public:
    StaticMesh()
        : VAO(0), EBO(0), indexCount(0), minBounds(0.0f), maxBounds(0.0f),
        size(0.0f), center(0.0f) {}
    ~StaticMesh();

    void setupMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    void loadFromImportedModel(ImportedModel& model);
    void bind() const;
    void draw() const;

    glm::vec3 getMinBounds() const { return minBounds; }
    glm::vec3 getMaxBounds() const { return maxBounds; }
    glm::vec3 getSize() const { return size; }
    glm::vec3 getCenter() const { return center; }

private:
    void cleanup();
};

#endif //DUCKENGINE_STATICMESH_H
