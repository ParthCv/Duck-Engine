#pragma once
#include "Shader.h"
#include "Texture.h"
#include "glad/glad.h"

class Cubemap {
public:
    GLuint id;
    int size;

    Cubemap();
    ~Cubemap();

    void createEmpty(int faceSize, GLenum format = GL_RGB16F);
    void fromHDR(Texture &texture, Shader &shader, int faceSize = 512);

    void bind(unsigned int textureSlot = 0) const;
    void unbind() const;

private:
    GLuint cubeVAO = 0, cubeVBO = 0;
    void setupCube();
    void renderCube();
};
