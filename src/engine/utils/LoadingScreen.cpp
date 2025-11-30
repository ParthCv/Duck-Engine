#include "LoadingScreen.h"

#include <filesystem>
#include <iostream>
#include <ostream>

void LoadingScreen::initialize(int width, int height) {
    screenWidth = width;
    screenHeight = height;

    bgTexture.loadFromFile("../assets/textures/loading.png", 0);

    if (!bgShader.loadFromFiles("../assets/shaders/loading_bg.vert", "../assets/shaders/loading_bg.frag")) {
        std::cerr << "Failed to load BG shader!" << std::endl;
    }

    if (!spinnerShader.loadFromFiles("../assets/shaders/loading_spinner.vert", "../assets/shaders/loading_spinner.frag")) {
        std::cerr << "Failed to load Spinner shader!" << std::endl;
    }


    float quad[] = {
        // positions   // UV
        -1.f, -1.f, 0.f, 0.f,
         1.f, -1.f, 1.f, 0.f,
         1.f,  1.f, 1.f, 1.f,
        -1.f, -1.f, 0.f, 0.f,
         1.f,  1.f, 1.f, 1.f,
        -1.f,  1.f, 0.f, 1.f
    };

    glGenVertexArrays(1, &bgVAO);
    glGenBuffers(1, &bgVBO);
    glBindVertexArray(bgVAO);
    glBindBuffer(GL_ARRAY_BUFFER, bgVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
    glEnableVertexAttribArray(1);


    float spinVerts[] = {
        -1, -1,
         1, -1,
         1,  1,
        -1, -1,
         1,  1,
        -1,  1
    };

    glGenVertexArrays(1, &spinnerVAO);
    glGenBuffers(1, &spinnerVBO);
    glBindVertexArray(spinnerVAO);
    glBindBuffer(GL_ARRAY_BUFFER, spinnerVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(spinVerts), spinVerts, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), 0);
    glEnableVertexAttribArray(0);
}

void LoadingScreen::render(float time) {
    glClearColor(0.07f, 0.07f, 0.09f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    bgShader.use();
    bgTexture.bind();
    bgShader.setInt("background", bgTexture.textureUnit);
    glBindVertexArray(bgVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    spinnerShader.use();
    spinnerShader.setFloat("time", time);
    spinnerShader.setVec2("resolution", glm::vec2(screenWidth, screenHeight));
    glBindVertexArray(spinnerVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisable(GL_BLEND);
}

void LoadingScreen::cleanup() {
    glDeleteVertexArrays(1, &bgVAO);
    glDeleteBuffers(1, &bgVBO);
    glDeleteVertexArrays(1, &spinnerVAO);
    glDeleteBuffers(1, &spinnerVBO);
}


