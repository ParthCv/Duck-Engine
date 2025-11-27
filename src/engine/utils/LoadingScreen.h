#pragma once
#include <glad/glad.h>
#include "../src/engine/renderer/Shader.h"
#include "../src/engine/renderer/Texture.h"


class LoadingScreen {
public:
    void initialize(int width, int height);
    void render(float time);
    void cleanup();

private:
    int screenWidth = 0, screenHeight = 0;

    GLuint bgVAO = 0, bgVBO = 0;
    GLuint spinnerVAO = 0, spinnerVBO = 0;

    Shader bgShader;
    Shader spinnerShader;

    Texture bgTexture;

};
