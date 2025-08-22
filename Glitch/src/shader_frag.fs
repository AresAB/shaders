#version 330 core

layout(location = 0) out vec3 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;

void main()
{
    vec3 color = texture(texture1, TexCoord).xyz;

    FragColor = vec3(TexCoord.x, 0.5, 0.5);
}