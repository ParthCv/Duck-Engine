#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include "../renderer/Shader.h"

class BitmapFont {
public:
    BitmapFont();
    ~BitmapFont();

    // Load a bitmap font texture
    bool load(const char* texturePath, int charWidth, int charHeight);

    // Render text at screen position (top-left corner)
    void renderText(const std::string& text, float x, float y, float scale,
                   glm::vec4 color, int screenWidth, int screenHeight);

    // Render text centered at position
    void renderTextCentered(const std::string& text, float x, float y, float scale,
                           glm::vec4 color, int screenWidth, int screenHeight);

    // Get text dimensions
    glm::vec2 measureText(const std::string& text, float scale) const;

    void cleanup();

    // Set the shader (UIManager needs to set it)
    void setShader(Shader* shader) { textShader = shader; }

private:
    GLuint textureID;
    GLuint VAO, VBO;
    int charWidth;
    int charHeight;
    int textureWidth;
    int textureHeight;
    int charsPerRow;
    Shader* textShader;

    void setupRenderData();
    glm::vec4 getCharUV(char c) const;
};
