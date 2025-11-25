#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform vec4 color;
uniform int renderMode; // 0 = solid, 1 = line, 2 = textured

void main()
{
    if (renderMode == 0 || renderMode == 1) {
        // Solid color rendering
        FragColor = color;
    } else if (renderMode == 2) {
        // Textured rendering (for future text rendering)
        // TODO: Sample from texture
        FragColor = color;
    }
}
