#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCord;
layout (location = 2) in float aFace;

out vec2 texCord;
out float face;

uniform mat4 matrix;
uniform mat4 model;
uniform mat4 rotation_y;
uniform mat4 rotation_z;
uniform mat4 view_pos;

void main() {
    texCord = aTexCord;
    face = aFace;
    gl_Position = matrix * rotation_z * rotation_y * view_pos * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
