
#include "StaticMesh.h"

void StaticMesh::setupMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    cleanup();

    indexCount = static_cast<GLsizei>(indices.size());

    // Generate buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &EBO);

    // VBOs[0] will be the main interleaved buffer
    GLuint mainVBO;
    glGenBuffers(1, &mainVBO);
    VBOs.push_back(mainVBO);

    // bind VAO
    glBindVertexArray(VAO);

    // Set Vertex Data
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    // Set Index Data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    // Normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    // TexCoords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

    // Unbind VAO
    glBindVertexArray(0);
}

void StaticMesh::loadFromImportedModel(ImportedModel &model) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    std::vector<glm::vec3> pos = model.getVertices();
    std::vector<glm::vec2> uvs = model.getTextureCoords();
    std::vector<glm::vec3> norms = model.getNormals();
    int numVertices = model.getNumVertices();

    // Interleave the data
    for (int i = 0; i < numVertices; i++) {
        Vertex v{};
        v.position = pos[i];

        if (i < uvs.size()) {
            v.texCoords = uvs[i];
        }
        else {
            v.texCoords = glm::vec2(0.0f);
        }

        if (i < norms.size()) {
            v.normal = norms[i];
        }
        else {
            v.normal = glm::vec3(0.0f, 0.0f, 1.0f);
        }

        vertices.push_back(v);
        indices.push_back(i); // ImportedModel already duplicated vertices for triangles, so we just index them sequentially
    }

    setupMesh(vertices, indices);
}

void StaticMesh::bind() const {
    glBindVertexArray(VAO);
}

void StaticMesh::draw() const {
    if (VAO == 0) return;
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
}

void StaticMesh::cleanup() {
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (EBO) glDeleteBuffers(1, &EBO);
    if (!VBOs.empty()) {
        glDeleteBuffers(static_cast<GLsizei>(VBOs.size()), VBOs.data());
        VBOs.clear();
    }
}

StaticMesh::~StaticMesh() {
    cleanup();
}

