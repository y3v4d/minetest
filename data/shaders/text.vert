#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCord;

out vec2 texCord;

uniform mat4 model;
uniform mat4 projection;

void main() {
    texCord = aTexCord;
    gl_Position = projection * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
