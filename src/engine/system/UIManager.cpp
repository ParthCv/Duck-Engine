#include <iostream>
#include <algorithm>
#include "UIManager.h"
#include "../game/GameStateManager.h"
#include "../core/managers/AudioManager.h"

UIManager::UIManager()
    : quadVAO(0)
    , quadVBO(0)
    , lineVAO(0)
    , lineVBO(0)
    , windowWidth(1920)
    , windowHeight(1080)
    , debugMode(false)
    , activeCrosshairIndex(-1)
{
}

UIManager::~UIManager() {
    shutdown();
}

void UIManager::initialize(int width, int height) {
    std::cout << "[UIManager] Initializing..." << std::endl;

    windowWidth = width;
    windowHeight = height;

    // Load UI shader
    if (!uiShader.loadFromFiles("../assets/shaders/ui.vert", "../assets/shaders/ui.frag")) {
        std::cerr << "[UIManager] Failed to load UI shaders" << std::endl;
        return;
    }

    // Set shader for font
    font.setShader(&uiShader);

    if (!font.load("../assets/fonts/font.png", 30, 30)) {
        std::cerr << "[UIManager] Failed to load bitmap font" << std::endl;
    }

    setupRenderingResources();

    std::cout << "[UIManager] Initialized successfully!" << std::endl;
}

void UIManager::setupRenderingResources() {
    // Setup quad for rendering rectangles/buttons
    float quadVertices[] = {
        // positions   // tex coords
        0.0f, 1.0f,    0.0f, 1.0f,
        1.0f, 0.0f,    1.0f, 0.0f,
        0.0f, 0.0f,    0.0f, 0.0f,

        0.0f, 1.0f,    0.0f, 1.0f,
        1.0f, 1.0f,    1.0f, 1.0f,
        1.0f, 0.0f,    1.0f, 0.0f
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);

    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);

    // Setup line rendering
    glGenVertexArrays(1, &lineVAO);
    glGenBuffers(1, &lineVBO);

    glBindVertexArray(lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    // Setup attributes matching the shader (Pos + Tex) just like Quad, so stride is 4 floats
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);
}

void UIManager::update(float deltaTime) {
    // Update buttons (hover/click detection)
    updateButtons();
    updateSliders();
    updateCrosshairPosition();
}

void UIManager::updateButtons() {
    for (auto& button : buttons) {
        if (!button.visible) continue;

        // Check if mouse is over button
        button.isHovered = isMouseOverElement(button);

        // Check for click
        if (button.isHovered && InputManager::isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
            button.isPressed = true;
            if (button.onClick) {
                button.onClick();
            }
        } else if (InputManager::isMouseButtonReleased(GLFW_MOUSE_BUTTON_LEFT)) {
            button.isPressed = false;
        }
    }
}

void UIManager::updateSliders() {
    glm::vec2 mousePos = InputManager::getMousePosition();
    bool mouseDown = InputManager::isMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT);
    bool mousePressed = InputManager::isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT);

    for (auto& slider : sliders) {
        if (!slider.visible) continue;

        // Check if started clicking on the slider
        if (mousePressed && isMouseOverElement(slider)) {
            slider.isDragging = true;
        }

        // Dragging, update value
        if (mouseDown && slider.isDragging) {
            glm::vec2 pos = getAnchoredPosition(slider);
            float relativeX = mousePos.x - pos.x;

            // Calculate percentage based on mouse position relative to slider width
            float percentage = glm::clamp(relativeX / slider.size.x, 0.0f, 1.0f);

            float newValue = slider.minVal + percentage * (slider.maxVal - slider.minVal);

            if (newValue != slider.value) {
                slider.value = newValue;
                // Call the callback to update audio immediately
                if (slider.onValueChanged) {
                    slider.onValueChanged(slider.value);
                }
            }
        }

        // Stop dragging if mouse released
        if (!mouseDown) {
            slider.isDragging = false;
        }
    }
}

bool UIManager::isMouseOverElement(const UIElement& element) {
    if (!element.visible) return false;

    glm::vec2 mousePos = InputManager::getMousePosition();
    glm::vec2 elementPos = getAnchoredPosition(element);

    return mousePos.x >= elementPos.x &&
           mousePos.x <= elementPos.x + element.size.x &&
           mousePos.y >= elementPos.y &&
           mousePos.y <= elementPos.y + element.size.y;
}

void UIManager::render() {
    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    uiShader.use();
    uiShader.setVec2("screenSize", glm::vec2(windowWidth, windowHeight));

    // Render panels first (background layer)
    for (const auto& panel : panels) {
        if (panel.visible) {
            renderPanel(panel);
        }
    }

    // Render sliders
    for (const auto& slider : sliders) {
        if (slider.visible) {
            renderSlider(slider);
        }
    }

    // Render buttons
    for (const auto& button : buttons) {
        if (button.visible) {
            renderButton(button);
        }
    }

    // Render text
    for (const auto& text : texts) {
        if (text.visible) {
            renderText(text);
        }
    }

    // Render crosshairs (foreground layer)
    for (const auto& crosshair : crosshairs) {
        if (crosshair.visible) {
            renderCrosshair(crosshair);
        }
    }

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}

void UIManager::renderButton(const UIButton& button) {
    // Determine button color based on state
    glm::vec4 currentColor = button.color;
    if (button.isPressed) {
        currentColor = button.pressedColor;
    } else if (button.isHovered) {
        currentColor = button.hoverColor;
    }

    // Render button background
    glm::vec2 position = getAnchoredPosition(button);
    renderQuad(position, button.size, currentColor);

    // Render button text (centered)
    if (!button.text.empty()) {
        glm::vec2 textPos = position + button.size * 0.5f;
        float scale = 24.0f / 30.0f;  // Adjust as needed
        font.renderTextCentered(button.text, textPos.x, textPos.y, scale, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), windowWidth, windowHeight);
    }
}

void UIManager::renderSlider(const UISlider& slider) {
    glm::vec2 pos = getAnchoredPosition(slider);

    // Render the background bar (track)
    renderQuad(pos, slider.size, slider.color);

    // Calculate handle position
    float range = slider.maxVal - slider.minVal;
    float percentage = (slider.value - slider.minVal) / range;

    float handleWidth = 10.0f;
    float handleHeight = slider.size.y + 10.0f; // Make it slightly taller

    glm::vec2 handleSize(handleWidth, handleHeight);

    // Center handle vertically and position horizontally
    glm::vec2 handlePos;
    handlePos.x = pos.x + (percentage * slider.size.x) - (handleWidth * 0.5f);
    handlePos.y = pos.y + (slider.size.y * 0.5f) - (handleHeight * 0.5f);

    renderQuad(handlePos, handleSize, slider.handleColor);
}

void UIManager::renderText(const UIText& text) {
    if (text.text.empty()) return;

    glm::vec2 position = getAnchoredPosition(text);
    float scale = text.fontSize / 30.0f;

    bool useCenteredRender = (text.anchor == UIAnchor::TOP_CENTER ||
                              text.anchor == UIAnchor::CENTER ||
                              text.anchor == UIAnchor::BOTTOM_CENTER);

    if (useCenteredRender) {

        font.renderTextCentered(text.text, position.x, position.y, scale, text.color, windowWidth, windowHeight);
    } else {
        if (text.anchor == UIAnchor::TOP_RIGHT ||
            text.anchor == UIAnchor::CENTER_RIGHT ||
            text.anchor == UIAnchor::BOTTOM_RIGHT) {

            glm::vec2 textSize = font.measureText(text.text, scale);
            position.x -= textSize.x; // Move left by text width
            }

        font.renderText(text.text, position.x, position.y, scale, text.color, windowWidth, windowHeight);
    }
}

void UIManager::renderPanel(const UIPanel& panel) {
    glm::vec2 position = getAnchoredPosition(panel);
    renderQuad(position, panel.size, panel.color);

    // Render border if enabled
    if (panel.hasBorder) {
        // Top
        renderLine(position, position + glm::vec2(panel.size.x, 0),
                   panel.borderColor, panel.borderThickness);
        // Right
        renderLine(position + glm::vec2(panel.size.x, 0), position + panel.size,
                   panel.borderColor, panel.borderThickness);
        // Bottom
        renderLine(position + panel.size, position + glm::vec2(0, panel.size.y),
                   panel.borderColor, panel.borderThickness);
        // Left
        renderLine(position + glm::vec2(0, panel.size.y), position,
                   panel.borderColor, panel.borderThickness);
    }
}

void UIManager::renderCrosshair(const UICrosshair& crosshair) {
    // Position is already set to mouse coords by updateCrosshairPosition
    glm::vec2 center = crosshair.position;

    // Render crosshair as 4 lines forming a cross
    float halfLength = crosshair.length * 0.5f;
    float gap = 5.0f; // Gap in the center

    // Horizontal line (left and right)
    renderLine(center - glm::vec2(halfLength, 0), center - glm::vec2(gap, 0), crosshair.color, crosshair.thickness);
    renderLine(center + glm::vec2(gap, 0), center + glm::vec2(halfLength, 0), crosshair.color, crosshair.thickness);

    // Vertical line (top and bottom)
    renderLine(center - glm::vec2(0, halfLength), center - glm::vec2(0, gap), crosshair.color, crosshair.thickness);
    renderLine(center + glm::vec2(0, gap), center + glm::vec2(0, halfLength), crosshair.color, crosshair.thickness);
}

void UIManager::renderQuad(glm::vec2 position, glm::vec2 size, glm::vec4 color) {
    uiShader.use();
    uiShader.setInt("renderMode", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    uiShader.setVec2("screenSize", glm::vec2(windowWidth, windowHeight));
    uiShader.setVec2("position", position);
    uiShader.setVec2("size", size);
    uiShader.setVec4("color", color);

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void UIManager::renderLine(glm::vec2 start, glm::vec2 end, glm::vec4 color, float thickness) {
    // Calculate line direction and perpendicular
    glm::vec2 dir = end - start;
    float length = glm::length(dir);
    if (length < 0.001f) return;

    dir = glm::normalize(dir);
    glm::vec2 perp = glm::vec2(-dir.y, dir.x) * (thickness * 0.5f);

    float lineQuad[] = {
        start.x - perp.x, start.y - perp.y,   0.0f, 0.0f,
        start.x + perp.x, start.y + perp.y,   1.0f, 0.0f,
        end.x + perp.x,   end.y + perp.y,     1.0f, 1.0f,

        start.x - perp.x, start.y - perp.y,   0.0f, 0.0f,
        end.x + perp.x,   end.y + perp.y,     1.0f, 1.0f,
        end.x - perp.x,   end.y - perp.y,     0.0f, 1.0f
    };

    uiShader.use();
    uiShader.setInt("renderMode", 1);  // Line mode - tells shader to skip transform
    uiShader.setVec2("screenSize", glm::vec2(windowWidth, windowHeight));
    uiShader.setVec4("color", color);

    glBindVertexArray(lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(lineQuad), lineQuad);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
}

void UIManager::renderSimpleText(const std::string& text, glm::vec2 position, float size, glm::vec4 color) {
    // Calculate scale
    float scale = size / 30.0f;

    // This is for button text - keep it centered
    font.renderTextCentered(text, position.x, position.y, scale, color, windowWidth, windowHeight);
}

glm::vec2 UIManager::getAnchoredPosition(const UIElement& element) {
    glm::vec2 basePos = element.position;

    // Apply anchor offset
    switch (element.anchor) {
        case UIAnchor::TOP_LEFT:
            // No offset needed
            break;
        case UIAnchor::TOP_CENTER:
            basePos.x += windowWidth * 0.5f - element.size.x * 0.5f;
            break;
        case UIAnchor::TOP_RIGHT:
            basePos.x = windowWidth - element.position.x - element.size.x;
            break;
        case UIAnchor::CENTER_LEFT:
            basePos.y += windowHeight * 0.5f - element.size.y * 0.5f;
            break;
        case UIAnchor::CENTER:
            basePos.x += windowWidth * 0.5f - element.size.x * 0.5f;
            basePos.y += windowHeight * 0.5f - element.size.y * 0.5f;
            break;
        case UIAnchor::CENTER_RIGHT:
            basePos.x = windowWidth - element.position.x - element.size.x;
            basePos.y += windowHeight * 0.5f - element.size.y * 0.5f;
            break;
        case UIAnchor::BOTTOM_LEFT:
            basePos.y = windowHeight - element.position.y - element.size.y;
            break;
        case UIAnchor::BOTTOM_CENTER:
            basePos.x += windowWidth * 0.5f - element.size.x * 0.5f;
            basePos.y = windowHeight - element.position.y - element.size.y;
            break;
        case UIAnchor::BOTTOM_RIGHT:
            basePos.x = windowWidth - element.position.x - element.size.x;
            basePos.y = windowHeight - element.position.y - element.size.y;
            break;
    }

    return basePos;
}

// === UI Element Management ===

int UIManager::addButton(const UIButton& button) {
    buttons.push_back(button);
    return buttons.size() - 1;
}

int UIManager::addText(const UIText& text) {
    texts.push_back(text);
    return texts.size() - 1;
}

int UIManager::addPanel(const UIPanel& panel) {
    panels.push_back(panel);
    return panels.size() - 1;
}

int UIManager::addCrosshair(const UICrosshair& crosshair) {
    crosshairs.push_back(crosshair);
    return crosshairs.size() - 1;
}

int UIManager::addSlider(const UISlider& slider) {
    sliders.push_back(slider);
    return sliders.size() - 1;
}

void UIManager::removeElement(const std::string& id) {
    // Remove from buttons
    buttons.erase(
        std::remove_if(buttons.begin(), buttons.end(),
            [&id](const UIButton& b) { return b.id == id; }),
        buttons.end()
    );

    // Remove from texts
    texts.erase(
        std::remove_if(texts.begin(), texts.end(),
            [&id](const UIText& t) { return t.id == id; }),
        texts.end()
    );

    // Remove from panels
    panels.erase(
        std::remove_if(panels.begin(), panels.end(),
            [&id](const UIPanel& p) { return p.id == id; }),
        panels.end()
    );

    // Remove from crosshairs
    crosshairs.erase(
        std::remove_if(crosshairs.begin(), crosshairs.end(),
            [&id](const UICrosshair& c) { return c.id == id; }),
        crosshairs.end()
    );

    // Remove from sliders
    sliders.erase(
        std::remove_if(sliders.begin(), sliders.end(),
            [&id](const UISlider& s) { return s.id == id; }),
        sliders.end()
    );
}

void UIManager::clearAll() {
    buttons.clear();
    texts.clear();
    panels.clear();
    crosshairs.clear();
    sliders.clear();
}

void UIManager::setElementVisible(const std::string& id, bool visible) {
    // Check buttons
    for (auto& button : buttons) {
        if (button.id == id) {
            button.visible = visible;
            return;
        }
    }

    // Check texts
    for (auto& text : texts) {
        if (text.id == id) {
            text.visible = visible;
            return;
        }
    }

    // Check panels
    for (auto& panel : panels) {
        if (panel.id == id) {
            panel.visible = visible;
            return;
        }
    }

    // Check crosshairs
    for (auto& crosshair : crosshairs) {
        if (crosshair.id == id) {
            crosshair.visible = visible;
            return;
        }
    }

    // Check sliders
    for (auto& slider : sliders) {
        if (slider.id == id) {
            slider.visible = visible;
            return;
        }
    }
}

void UIManager::setElementText(const std::string& id, const std::string& newText) {
    // Check buttons
    for (auto& button : buttons) {
        if (button.id == id) {
            button.text = newText;
            return;
        }
    }

    // Check texts
    for (auto& text : texts) {
        if (text.id == id) {
            text.text = newText;
            return;
        }
    }
}

void UIManager::onWindowResize(int newWidth, int newHeight) {
    windowWidth = newWidth;
    windowHeight = newHeight;
    std::cout << "[UIManager] Window resized to " << windowWidth << "x" << windowHeight << std::endl;
}

void UIManager::shutdown() {
    if (quadVAO != 0) {
        glDeleteVertexArrays(1, &quadVAO);
        glDeleteBuffers(1, &quadVBO);
        quadVAO = 0;
        quadVBO = 0;
    }

    if (lineVAO != 0) {
        glDeleteVertexArrays(1, &lineVAO);
        glDeleteBuffers(1, &lineVBO);
        lineVAO = 0;
        lineVBO = 0;
    }

    font.cleanup();

    clearAll();
    std::cout << "[UIManager] Shutdown complete" << std::endl;
}

// === State-specific UI Setup ===
void UIManager::setupMenuUI(GameStateManager* stateManager) {
    clearMenuUI();

    std::cout << "[UIManager] Setting up MENU UI" << std::endl;

    // Title text
    UIText title;
    title.id = "menu_title";
    title.text = "DUCK HUNT 3D";
    title.position = glm::vec2(0, 150);
    title.fontSize = 48.0f;
    title.anchor = UIAnchor::TOP_CENTER;
    title.color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
    addText(title);

    // Play button
    UIButton playButton;
    playButton.id = "menu_play";
    playButton.text = "PLAY";
    playButton.position = glm::vec2(0, 0);
    playButton.size = glm::vec2(200, 60);
    playButton.anchor = UIAnchor::CENTER;
    playButton.onClick = [stateManager]() {
        std::cout << "[UIManager] Play button clicked!" << std::endl;
        if (stateManager) {
            stateManager->setState(GameState::PLAYING);
        }
    };
    addButton(playButton);

    // Options button
    UIButton optionsButton;
    optionsButton.id = "menu_options";
    optionsButton.text = "OPTIONS";
    optionsButton.position = glm::vec2(0, 80);
    optionsButton.size = glm::vec2(200, 60);
    optionsButton.anchor = UIAnchor::CENTER;
    optionsButton.onClick = [stateManager]() {
        std::cout << "[UIManager] Options button clicked!" << std::endl;
        if (stateManager) {
            stateManager->setState(GameState::OPTIONS);
        }
    };
    addButton(optionsButton);

    // Quit button
    UIButton quitButton;
    quitButton.id = "menu_quit";
    quitButton.text = "QUIT";
    quitButton.position = glm::vec2(0, 160);
    quitButton.size = glm::vec2(200, 60);
    quitButton.anchor = UIAnchor::CENTER;
    quitButton.onClick = []() {
        std::cout << "[UIManager] Quit button clicked!" << std::endl;
        // TODO: Signal engine to close
    };
    addButton(quitButton);
}

void UIManager::setupPlayingUI() {
    clearPlayingUI();

    std::cout << "[UIManager] Setting up PLAYING UI" << std::endl;

    // Crosshair
    UICrosshair crosshair;
    crosshair.id = "crosshair";
    crosshair.length = 30.0f;
    crosshair.thickness = 2.0f;
    crosshair.color = glm::vec4(0.0f, 1.0f, 0.0f, 0.8f); // Green Crosshair
    crosshair.anchor = UIAnchor::CENTER;
    crosshair.position = glm::vec2(0, 0);
    activeCrosshairIndex = addCrosshair(crosshair);

    // Score text (top-left)
    UIText scoreText;
    scoreText.id = "score_text";
    scoreText.text = "SCORE: 0";
    scoreText.position = glm::vec2(20, 20);
    scoreText.fontSize = 24.0f;  // Adjust size as needed
    scoreText.anchor = UIAnchor::TOP_LEFT;
    scoreText.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    addText(scoreText);

    // Ammo text (BOTTOM-LEFT)
    UIText ammoText;
    ammoText.id = "ammo_text";
    ammoText.text = "AMMO: 10";
    ammoText.position = glm::vec2(20, 20);  // 20px from bottom-left
    ammoText.fontSize = 24.0f;
    ammoText.anchor = UIAnchor::BOTTOM_LEFT;  // Changed from TOP_LEFT
    ammoText.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    addText(ammoText);

    // Round text (top-right)
    UIText roundText;
    roundText.id = "round_text";
    roundText.text = "ROUND: 1";
    roundText.position = glm::vec2(20, 20);  // 20px from top-right corner
    roundText.fontSize = 24.0f;
    roundText.anchor = UIAnchor::TOP_RIGHT;
    roundText.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    addText(roundText);
}

void UIManager::setupPausedUI(GameStateManager* stateManager) {
    std::cout << "[UIManager] Setting up PAUSED UI" << std::endl;

    // Semi-transparent overlay
    UIPanel overlay;
    overlay.id = "pause_overlay";
    overlay.position = glm::vec2(0, 0);
    overlay.size = glm::vec2(windowWidth, windowHeight);
    overlay.color = glm::vec4(0.0f, 0.0f, 0.0f, 0.5f);
    overlay.anchor = UIAnchor::TOP_LEFT;
    addPanel(overlay);

    // Pause title
    UIText pauseTitle;
    pauseTitle.id = "pause_title";
    pauseTitle.text = "PAUSED";
    pauseTitle.position = glm::vec2(0, -100);
    pauseTitle.fontSize = 48.0f;
    pauseTitle.anchor = UIAnchor::CENTER;
    pauseTitle.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    addText(pauseTitle);

    // Resume button
    UIButton resumeButton;
    resumeButton.id = "pause_resume";
    resumeButton.text = "RESUME";
    resumeButton.position = glm::vec2(0, 0);
    resumeButton.size = glm::vec2(200, 60);
    resumeButton.anchor = UIAnchor::CENTER;
    resumeButton.onClick = [stateManager]() {
        if (stateManager) {
            stateManager->resumeGame();
        }
    };
    addButton(resumeButton);

    // Menu button
    UIButton menuButton;
    menuButton.id = "pause_menu";
    menuButton.text = "MENU";
    menuButton.position = glm::vec2(0, 80);
    menuButton.size = glm::vec2(200, 60);
    menuButton.anchor = UIAnchor::CENTER;
    menuButton.onClick = [stateManager]() {
        if (stateManager) {
            stateManager->returnToMenu();
        }
    };
    addButton(menuButton);
}

void UIManager::setupGameOverUI(GameStateManager* stateManager) {
    clearGameOverUI();

    std::cout << "[UIManager] Setting up GAME OVER UI" << std::endl;

    // Game Over title
    UIText gameOverTitle;
    gameOverTitle.id = "gameover_title";
    gameOverTitle.text = "GAME OVER";
    gameOverTitle.position = glm::vec2(0, -150);
    gameOverTitle.fontSize = 72.0f;
    gameOverTitle.anchor = UIAnchor::CENTER;
    gameOverTitle.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    addText(gameOverTitle);

    // Final score
    UIText finalScore;
    finalScore.id = "gameover_score";
    finalScore.text = "FINAL SCORE: 0";
    finalScore.position = glm::vec2(0, -50);
    finalScore.fontSize = 48.0f;
    finalScore.anchor = UIAnchor::CENTER;
    finalScore.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    addText(finalScore);

    // Restart button
    UIButton restartButton;
    restartButton.id = "gameover_restart";
    restartButton.text = "RESTART";
    restartButton.position = glm::vec2(0, 80);
    restartButton.size = glm::vec2(200, 60);
    restartButton.anchor = UIAnchor::CENTER;
    restartButton.onClick = [stateManager]() {
        if (stateManager) {
            stateManager->restartGame();
        }
    };
    addButton(restartButton);

    // Menu button
    UIButton menuButton;
    menuButton.id = "gameover_menu";
    menuButton.text = "MENU";
    menuButton.position = glm::vec2(0, 160);
    menuButton.size = glm::vec2(200, 60);
    menuButton.anchor = UIAnchor::CENTER;
    menuButton.onClick = [stateManager]() {
        if (stateManager) {
            stateManager->returnToMenu();
        }
    };
    addButton(menuButton);
}

void UIManager::setupOptionsUI(GameStateManager* stateManager) {
    clearAll();

    std::cout << "[UIManager] Setting up OPTIONS UI" << std::endl;

    // Options title
    UIText optionsTitle;
    optionsTitle.id = "options_title";
    optionsTitle.text = "OPTIONS";
    optionsTitle.position = glm::vec2(0, 100);
    optionsTitle.fontSize = 64.0f;
    optionsTitle.anchor = UIAnchor::TOP_CENTER;
    optionsTitle.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    addText(optionsTitle);

    // Volume Label
    UIText volumeLabel;
    volumeLabel.id = "volume_label";
    volumeLabel.text = "Master Volume";
    volumeLabel.position = glm::vec2(0, -40); // Slightly above center
    volumeLabel.fontSize = 32.0f;
    volumeLabel.anchor = UIAnchor::CENTER;
    volumeLabel.color = glm::vec4(0.9f, 0.9f, 0.9f, 1.0f);
    addText(volumeLabel);

    // Master Volume Slider
    UISlider volumeSlider;
    volumeSlider.id = "master_volume_slider";
    volumeSlider.position = glm::vec2(0, 10);
    volumeSlider.size = glm::vec2(400, 30);
    volumeSlider.anchor = UIAnchor::CENTER;
    volumeSlider.minVal = 0.0f;
    volumeSlider.maxVal = 1.0f;
    volumeSlider.value = AudioManager::Get().GetMasterVolume();

    // Callback to update AudioManager
    volumeSlider.onValueChanged = [](float val) {
        // Assuming AudioManager singleton access
        AudioManager::Get().SetMasterVolume(val);
    };

    addSlider(volumeSlider);

    // Back button
    UIButton backButton;
    backButton.id = "options_back";
    backButton.text = "BACK";
    backButton.position = glm::vec2(0, 200);
    backButton.size = glm::vec2(200, 60);
    backButton.anchor = UIAnchor::CENTER;
    backButton.onClick = [stateManager]() {
        if (stateManager) {
            stateManager->returnToMenu();
        }
    };
    addButton(backButton);
}

void UIManager::updateCrosshairPosition() {
    glm::vec2 center(windowWidth / 2.0f, windowHeight / 2.0f);

    for (auto& crosshair : crosshairs) {
        if (crosshair.visible) {
            crosshair.position = center;
            crosshair.anchor = UIAnchor::CENTER; // Ensure logic treats it as center
        }
    }
}

void UIManager::clearMenuUI() {
    removeElement("menu_title");
    removeElement("menu_play");
    removeElement("menu_options");
    removeElement("menu_quit");
}

void UIManager::clearPlayingUI() {
    removeElement("crosshair");
    removeElement("score_text");
    removeElement("ammo_text");
    removeElement("round_text");
}

void UIManager::clearPausedUI() {
    removeElement("pause_overlay");
    removeElement("pause_title");
    removeElement("pause_resume");
    removeElement("pause_menu");
}

void UIManager::clearGameOverUI() {
    removeElement("gameover_title");
    removeElement("gameover_score");
    removeElement("gameover_restart");
    removeElement("gameover_menu");
}

void UIManager::setCursorVisible(GLFWwindow* window, bool visible) {
    if (window) {
        if (visible) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            std::cout << "[UIManager] Cursor mode set to NORMAL" << std::endl;
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            std::cout << "[UIManager] Cursor mode set to DISABLED" << std::endl;
        }
    }
}
