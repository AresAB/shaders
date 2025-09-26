#version 330 core

layout(location = 0) out vec3 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;
uniform float time;

void main()
{
    FragColor = texture(texture1, sin(TexCoord + time * 0.5)).xyz;
}