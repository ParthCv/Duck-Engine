
#ifndef DUCKENGINE_IMPORTEDMODEL_H
#define DUCKENGINE_IMPORTEDMODEL_H
#include <vector>
#include <glm/glm.hpp>
#include <string>

class ImportedModel
{
private:
    int numVertices;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> texCoords;
    std::vector<glm::vec3> normals;

public:
    ImportedModel(const std::string& filename);

    // accessors
    int getNumVertices() const { return numVertices; }
    std::vector<glm::vec3> getVertices() const { return vertices; }
    std::vector<glm::vec2> getTextureCoords() const { return texCoords; }
    std::vector<glm::vec3> getNormals() const { return normals; }
};

class ModelImporter
{
private:
    // values as read in from OBJ file
    std::vector<float> vertVals;
    std::vector<float> stVals;
    std::vector<float> normVals;
    // values stored for later use as vertex attributes
    std::vector<float> triangleVerts;
    std::vector<float> textureCoords;
    std::vector<float> normals;

public:
    ModelImporter();
    void parseOBJ(const std::string& filename);

    // accessors
    int getNumVertices() { return (int)(triangleVerts.size() / 3); }
    std::vector<float> getVertices() { return triangleVerts; }
    std::vector<float> getTextureCoordinates() { return textureCoords; }
    std::vector<float> getNormals() { return normals; }
};


#endif //DUCKENGINE_IMPORTEDMODEL_H
