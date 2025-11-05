#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;

void main() {
    // For now, just show albedo (textured cube)
    FragColor = texture(gAlbedo, TexCoords);
}