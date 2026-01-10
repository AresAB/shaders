#version 330 core

layout (location = 0) out vec3 OUT_COLOR;

in vec2 UV;

uniform sampler2D TEX_1;

void main() {
    OUT_COLOR = texture(TEX_1, UV).xyz;
    //OUT_COLOR = vec3(0.2, 0.5, 0.7);
}