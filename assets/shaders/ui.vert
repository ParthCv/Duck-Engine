#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform vec2 screenSize;  // Window dimensions
uniform vec2 position;    // Position in screen space
uniform vec2 size;        // Size in screen space
uniform int renderMode;   // 0 = solid, 1 = line, 2 = textured

void main()
{
    vec2 finalPos;

    if (renderMode == 1) {
        // Line mode - aPos is already in screen space
        finalPos = aPos;
    } else {
        // Normal mode - transform from unit quad to screen space
        vec2 scaledPos = aPos * size;
        finalPos = position + scaledPos;
    }

    // Convert to NDC
    vec2 ndc;
    ndc.x = (finalPos.x / screenSize.x) * 2.0 - 1.0;
    ndc.y = 1.0 - (finalPos.y / screenSize.y) * 2.0;

    gl_Position = vec4(ndc, 0.0, 1.0);
    TexCoord = aTexCoord;
}
