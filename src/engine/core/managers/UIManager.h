#pragma once
#include "../../renderer/Shader.h"
#include "../../renderer/BitmapFont.h"
#include "../src/engine/renderer/Texture.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <functional>

// Forward declarations
class UIStateManager;
class World;
enum class DuckState;

// UI Element Types
enum class UIElementType {
    TEXT,
    BUTTON,
    PANEL,
    IMAGE,
    CROSSHAIR,
    SLIDER
};

// Anchor positions for UI elements
enum class UIAnchor {
    TOP_LEFT,
    TOP_CENTER,
    TOP_RIGHT,
    CENTER_LEFT,
    CENTER,
    CENTER_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_CENTER,
    BOTTOM_RIGHT
};

// Base UI Element structure
struct UIElement {
    UIElementType type;
    glm::vec2 position;      // Position in screen space (pixels)
    glm::vec2 size;          // Size in pixels
    glm::vec4 color;         // RGBA color
    UIAnchor anchor;         // Anchor point
    bool visible;            // Whether to render this element
    std::string id;          // Unique identifier

    UIElement()
        : type(UIElementType::PANEL)
        , position(0.0f, 0.0f)
        , size(100.0f, 100.0f)
        , color(1.0f, 1.0f, 1.0f, 1.0f)
        , anchor(UIAnchor::TOP_LEFT)
        , visible(true)
    {}
};

// Button element with callback
struct UIButton : UIElement {
    std::string text;
    glm::vec4 hoverColor;
    glm::vec4 pressedColor;
    std::function<void()> onClick;
    bool isHovered;
    bool isPressed;

    UIButton()
        : isHovered(false)
        , isPressed(false)
    {
        type = UIElementType::BUTTON;
        color = glm::vec4(0.3f, 0.3f, 0.3f, 0.8f);
        hoverColor = glm::vec4(0.5f, 0.5f, 0.5f, 0.9f);
        pressedColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
    }
};

// Slider element
struct UISlider : UIElement {
    float minVal;
    float maxVal;
    float value;
    glm::vec4 handleColor;
    bool isDragging;
    std::function<void(float)> onValueChanged;

    UISlider()
        : minVal(0.0f)
        , maxVal(1.0f)
        , value(0.5f)
        , handleColor(0.8f, 0.8f, 0.8f, 1.0f)
        , isDragging(false)
    {
        type = UIElementType::SLIDER;
        color = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f); // Background track color
        size = glm::vec2(300.0f, 20.0f);
    }
};

// Text element
struct UIText : UIElement {
    std::string text;
    float fontSize;
    bool centered;

    UIText()
        : fontSize(24.0f)
        , centered(false)
    {
        type = UIElementType::TEXT;
        size = glm::vec2(0, 0); // Auto-size based on text
    }
};

// Panel/Rectangle element
struct UIPanel : UIElement {
    bool hasBorder;
    glm::vec4 borderColor;
    float borderThickness;

    UIPanel()
        : hasBorder(false)
        , borderColor(1.0f, 1.0f, 1.0f, 1.0f)
        , borderThickness(2.0f)
    {
        type = UIElementType::PANEL;
    }
};

// Crosshair element (for Duck Hunt)
struct UICrosshair : UIElement {
    float thickness;
    float length;

    UICrosshair()
        : thickness(3.0f)
        , length(20.0f)
    {
        type = UIElementType::CROSSHAIR;
        anchor = UIAnchor::CENTER;
        color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    }
};

class UIManager {
public:
    UIManager();
    ~UIManager();

    // Initialize with window dimensions
    void initialize(int windowWidth, int windowHeight);

    // Update UI (handle input, animations, etc.)
    void update(float deltaTime);

    // Render all UI elements
    void render();

    // Cleanup
    void shutdown();

    // Window resize handling
    void onWindowResize(int newWidth, int newHeight);

    // === UI Element Management ===
    void renderSprite(glm::vec2 position, glm::vec2 size, glm::vec4 color,
                  int sprite_x, int sprite_y, int sprite_w, int sprite_h);

    // Add UI elements (returns index for later access)
    int addButton(const UIButton& button);
    int addText(const UIText& text);
    int addPanel(const UIPanel& panel);
    int addCrosshair(const UICrosshair& crosshair);
    int addSlider(const UISlider& slider);

    // Remove UI element by ID
    void removeElement(const std::string& id);

    // Clear all UI elements
    void clearAll();

    // Show/hide elements
    void setElementVisible(const std::string& id, bool visible);
    void setElementText(const std::string& id, const std::string& newText);

    // === State-specific UI Setup ===
    void setupMenuUI(UIStateManager* stateManager, std::function<void()> onQuit = nullptr);
    void setupPlayingUI();
    void setupPausedUI(UIStateManager* stateManager);
    void setupGameOverUI(UIStateManager* stateManager);
    void setupOptionsUI(UIStateManager* stateManager);

    // Needs constant update
    void updateCrosshairPosition();

    // Clear state-specific UI
    void clearMenuUI();
    void clearPlayingUI();
    void clearPausedUI();
    void clearGameOverUI();

    // Helper to control GLFW cursor visibility
    void setCursorVisible(GLFWwindow* window, bool visible);

    // Utility
    void setDebugMode(bool enabled) { debugMode = enabled; }

    void renderLoadingScreen();

    void resetDuckStates();

    void renderDuckStatusBar();

    void renderAmmoBar();

    void renderScore();

    void renderRound();

    // ADD THIS METHOD
    void setWorldContext(World* world) { worldContext = world; }

private:
    // OpenGL rendering setup
    Shader uiShader;
    unsigned int quadVAO, quadVBO;
    unsigned int lineVAO, lineVBO;

    // Window dimensions
    int windowWidth;
    int windowHeight;

    Texture spriteAtlas;

    // UI Elements storage
    std::vector<UIButton> buttons;
    std::vector<UIText> texts;
    std::vector<UIPanel> panels;
    std::vector<UICrosshair> crosshairs;
    std::vector<UISlider> sliders;

    int activeCrosshairIndex;

    BitmapFont font;

    // Debug mode
    bool debugMode;

    // Flag to control HUD rendering
    bool showGameplayHUD;

    World* worldContext;

    struct SpriteCoords {
        int x, y, w, h;
    };

    SpriteCoords sprite_bullet = {212, 235, 32, 32};
    SpriteCoords sprite_duck_not_spawned = {0, 120, 34, 34};
    SpriteCoords sprite_duck_spawned = {170, 120, 34, 34};
    SpriteCoords sprite_duck_hit = {130, 238, 34, 34};
    SpriteCoords sprite_duck_escaped = {300, 156, 34, 34};

    std::string formatScore(int score);

    // === Internal Rendering Methods ===
    void setupRenderingResources();


    void renderButton(const UIButton& button);
    void renderText(const UIText& text);
    void renderPanel(const UIPanel& panel);
    void renderCrosshair(const UICrosshair& crosshair);
    void renderSlider(const UISlider& slider);

    // Render primitives
    void renderQuad(glm::vec2 position, glm::vec2 size, glm::vec4 color);
    void renderLine(glm::vec2 start, glm::vec2 end, glm::vec4 color, float thickness);

    // === Input Handling ===
    void updateButtons();
    void updateSliders();
    bool isMouseOverElement(const UIElement& element);

    // === Coordinate Conversion ===
    glm::vec2 getAnchoredPosition(const UIElement& element);
    glm::vec2 normalizedToScreen(glm::vec2 normalized); // NDC [-1,1] to screen [0,width/height]
    glm::vec2 screenToNormalized(glm::vec2 screen);     // Screen to NDC

    // Simple text rendering (using quads for now - can be upgraded later)
    void renderSimpleText(const std::string& text, glm::vec2 position, float size, glm::vec4 color);
};