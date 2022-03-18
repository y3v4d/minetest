#version 330 core

out vec4 FragColor;

uniform sampler2DArray ourTexture;
uniform int layer;

in vec2 texCord;

void main() {
   FragColor = texture(ourTexture, vec3(texCord, layer));
}
