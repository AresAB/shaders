#version 330 core

layout(location = 0) out vec3 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;

void main()
{
    // waves
    vec2 offset;
    offset = vec2(sin(TexCoord.y * 30) * 0.01, sin(TexCoord.x * 30) * 0.01);
    offset = vec2(cos(TexCoord.y * 30) * 0.01, sin(TexCoord.x * 30) * 0.01);

    // shockwave
    /*vec2 dir = TexCoord - vec2(0.5);
    float d = length(dir) - 0.35;
    d *= 1 - smoothstep(0., 0.05, abs(d));
    offset = normalize(dir) * d;*/

    FragColor = texture(texture1, TexCoord + offset).xyz;
}