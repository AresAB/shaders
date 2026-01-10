#version 330 core

layout (location = 0) in vec3 ABS_POS;
layout (location = 1) in vec2 TEX_COORD;

out vec2 UV;

void main() {
    gl_Position = vec4(ABS_POS, 1.0);
    UV = TEX_COORD;
}