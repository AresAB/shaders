#version 330 core

layout(location = 0) out vec3 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;

void main()
{
    // default is 0
    float brightness = 0.;
    // default is 1
    float contrast = 1.;
    // default is 1
    float saturation = 1.;
    // default is 1
    float gamma = 1;

    vec3 grayscale = vec3(0.2989, 0.589, 0.114);

    vec3 color = texture(texture1, TexCoord).xyz;

    color = clamp(contrast * (color - 0.5) + 0.5 + brightness, 0., 1.);

    color = mix(vec3(dot(color, grayscale)), color, saturation);

    color.x = pow(color.x, gamma);
    color.y = pow(color.y, gamma);
    color.z = pow(color.z, gamma);

    FragColor = color;
}