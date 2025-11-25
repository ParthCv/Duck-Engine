#include "BitmapFont.h"
#include <iostream>
#include <stb_image.h>

BitmapFont::BitmapFont()
    : textureID(0)
    , VAO(0)
    , VBO(0)
    , charWidth(8)
    , charHeight(8)
    , textureWidth(128)
    , textureHeight(128)
    , charsPerRow(16)
    , textShader(nullptr)
{
}

BitmapFont::~BitmapFont() {
    cleanup();
}

bool BitmapFont::load(const char* texturePath, int charW, int charH) {
    charWidth = charW;
    charHeight = charH;

    // Load texture using stb_image
    int width, height, channels;
    stbi_set_flip_vertically_on_load(false);
    unsigned char* data = stbi_load(texturePath, &width, &height, &channels, 0);

    if (!data) {
        std::cerr << "[BitmapFont] Failed to load font texture: " << texturePath << std::endl;
        return false;
    }

    textureWidth = width;
    textureHeight = height;
    charsPerRow = textureWidth / charWidth;

    // Create OpenGL texture
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    // Set texture parameters - use NEAREST for crisp pixel art
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);

    setupRenderData();

    std::cout << "[BitmapFont] Loaded font: " << texturePath
              << " (" << width << "x" << height << ", " << channels << " channels)" << std::endl;

    return true;
}

void BitmapFont::setupRenderData() {
    // Setup VAO/VBO for rendering quads
    float vertices[] = {
        // pos      // tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

glm::vec4 BitmapFont::getCharUV(char c) const {
    int charsPerCol = textureHeight / charHeight;

    unsigned char index = (unsigned char)c;

    // Map ASCII characters to grid positions
    if (index < 32) {
        index = 32;
    }

    index -= 32;

    int col = index % charsPerRow;
    int row = index / charsPerRow;

    float u0 = (float)col / charsPerRow;
    float v0 = (float)row / charsPerCol;
    float u1 = (float)(col + 1) / charsPerRow;
    float v1 = (float)(row + 1) / charsPerCol;

    return glm::vec4(u0, v0, u1, v1);
}

void BitmapFont::renderText(const std::string& text, float x, float y, float scale,
                           glm::vec4 color, int screenWidth, int screenHeight) {
    if (!textShader) {
        std::cerr << "[BitmapFont] ERROR: No shader set!" << std::endl;
        return;
    }

    textShader->use();
    textShader->setInt("renderMode", 2); // Textured mode
    textShader->setVec2("screenSize", glm::vec2(screenWidth, screenHeight));
    textShader->setVec4("color", color);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    textShader->setInt("fontTexture", 0);

    glBindVertexArray(VAO);

    float currentX = x;
    float currentY = y;
    float scaledWidth = charWidth * scale;
    float scaledHeight = charHeight * scale;

    for (char c : text) {
        // Handle newlines
        if (c == '\n') {
            currentX = x;
            currentY += scaledHeight;
            continue;
        }

        glm::vec4 uv = getCharUV(c);

        // Update VBO with character-specific UVs
        float vertices[] = {
            0.0f, 1.0f, uv.x, uv.w,
            1.0f, 0.0f, uv.z, uv.y,
            0.0f, 0.0f, uv.x, uv.y,

            0.0f, 1.0f, uv.x, uv.w,
            1.0f, 1.0f, uv.z, uv.w,
            1.0f, 0.0f, uv.z, uv.y
        };

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        textShader->setVec2("position", glm::vec2(currentX, currentY));
        textShader->setVec2("size", glm::vec2(scaledWidth, scaledHeight));

        glDrawArrays(GL_TRIANGLES, 0, 6);

        currentX += scaledWidth;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void BitmapFont::renderTextCentered(const std::string& text, float x, float y, float scale,
                                   glm::vec4 color, int screenWidth, int screenHeight) {
    glm::vec2 textSize = measureText(text, scale);
    float centeredX = x - textSize.x * 0.5f;
    float centeredY = y - textSize.y * 0.5f;
    renderText(text, centeredX, centeredY, scale, color, screenWidth, screenHeight);
}

glm::vec2 BitmapFont::measureText(const std::string& text, float scale) const {
    float width = 0.0f;
    float height = charHeight * scale;
    int lineCount = 1;
    int maxLineWidth = 0;
    int currentLineWidth = 0;

    for (char c : text) {
        if (c == '\n') {
            lineCount++;
            if (currentLineWidth > maxLineWidth) {
                maxLineWidth = currentLineWidth;
            }
            currentLineWidth = 0;
        } else {
            currentLineWidth++;
        }
    }

    // Check last line
    if (currentLineWidth > maxLineWidth) {
        maxLineWidth = currentLineWidth;
    }

    // If no newlines, just use total character count
    if (lineCount == 1) {
        maxLineWidth = text.length();
    }

    width = maxLineWidth * charWidth * scale;
    return glm::vec2(width, height * lineCount);
}

void BitmapFont::cleanup() {
    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        VAO = 0;
        VBO = 0;
    }
    if (textureID != 0) {
        glDeleteTextures(1, &textureID);
        textureID = 0;
    }
    std::cout << "[BitmapFont] Cleanup complete" << std::endl;
}
