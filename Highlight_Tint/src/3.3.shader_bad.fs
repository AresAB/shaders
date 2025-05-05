#version 330 core

layout(location = 0) out vec3 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;

void main()
{
    // This will affect all 3 colors, aka num_colors = 2 actually allows for 6 colors
    float num_colors = 2;

    vec3 color = texture(texture1, TexCoord).xyz;
    float lumin = dot(vec3(0.2989, 0.589, 0.114), color);

    float is_highlight1 = floor(floor(lumin * (num_colors - 1.) + 0.5) / (num_colors - 1.));
    float is_highlight2 = floor((1. - lumin) * (num_colors - 1.) + 0.5) / (num_colors - 1.);

    vec3 highlight1 = vec3(1., 0.07, 0.72);
    vec3 highlight2 = vec3(0.18, 0.84, 0.96);

    FragColor = color * (1 - is_highlight1);
}