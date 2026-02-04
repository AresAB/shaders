#version 330 core

layout(location = 0) out vec3 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;
uniform vec3 Color;
uniform float Lerp;

void main()
{
    FragColor = mix(texture(texture1, TexCoord).xyz, Color, Lerp);
}
