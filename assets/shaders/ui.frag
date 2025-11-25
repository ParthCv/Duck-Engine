#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform vec4 color;
uniform int renderMode; // 0 = solid, 1 = line, 2 = textured
uniform sampler2D fontTexture;

void main()
{
    if (renderMode == 0 || renderMode == 1) {
        // Solid color rendering
        FragColor = color;
    } else if (renderMode == 2) {
        // Textured rendering (for bitmap font)
        vec4 texColor = texture(fontTexture, TexCoord);

        // Multiply texture color by tint color
        FragColor = texColor * color;

        // Discard fully transparent pixels
        if (FragColor.a < 0.1)
        discard;
    }
}
