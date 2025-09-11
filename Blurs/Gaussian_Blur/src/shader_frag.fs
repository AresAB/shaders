#version 330 core

layout(location = 0) out vec3 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;

void main()
{
    FragColor = vec3(0.5, 0.25, 0.25) + texture(texture1, TexCoord).xyz;
}