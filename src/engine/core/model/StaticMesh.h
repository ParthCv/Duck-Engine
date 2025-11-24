
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
private:
    GLuint VAO;
    std::vector<GLuint> VBOs;
    GLuint EBO;
    GLsizei indexCount;

public:
    StaticMesh() : VAO(0), EBO(0), indexCount(0) {}
    ~StaticMesh();

    void setupMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    void loadFromImportedModel(ImportedModel& model);
    void bind() const;
    void draw() const;

private:
    void cleanup();
};

#endif //DUCKENGINE_STATICMESH_H
