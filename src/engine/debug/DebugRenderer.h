#pragma once
#include <glad/glad.h>
#include "../renderer/Shader.h"

class DebugRenderer {
public:
    static DebugRenderer& getInstance() {
        static DebugRenderer instance;
        return instance;
    }

    // Delete copy constructor and assignment operator
    DebugRenderer(const DebugRenderer&) = delete;
    DebugRenderer& operator=(const DebugRenderer&) = delete;

    void init(); // Call once at startup
    void drawBox(Shader basicShader, const glm::vec3& position, const glm::vec3& size, const glm::vec3& color = glm::vec3(0, 1, 0));
    void drawLine(::Shader basicShader, const glm::vec3 & start, const glm::vec3 & end, const bool hit);
    void cleanup(); // Call at shutdown

private:
    DebugRenderer() = default;  // Private constructor
    ~DebugRenderer() = default;

    void initCubeGeometry();
    void initLineGeometry();

    GLuint cubeVAO = 0, cubeVBO = 0;
    GLuint lineVAO = 0, lineVBO = 0;
};