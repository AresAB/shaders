#version 330 core

layout(location = 0) out vec3 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;

void main()
{
    // This will affect all 3 colors, aka num_colors = 2 actually allows for 6 colors
    float num_colors = 5;

    vec3 color = texture(texture1, TexCoord).xyz;

    // vec3 gray_scale = vec3(0.2989, 0.589, 0.114);
    // color = vec3(dot(gray_scale, color));

    color = floor(color * (num_colors - 1.) + 0.5) / (num_colors - 1.);

    FragColor = color;
}