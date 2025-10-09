#version 330 core

layout(location = 0) out vec3 FragColor;

in vec2 TexCoord;

uniform vec3 light_col;

void main()
{
    FragColor = light_col;
}