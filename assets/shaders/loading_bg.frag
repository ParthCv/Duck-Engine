#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D background;

void main() {
    FragColor = texture(background, TexCoord);
}
