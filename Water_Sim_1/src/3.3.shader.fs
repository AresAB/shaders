#version 330 core

layout(location = 0) out vec3 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 View;

uniform sampler2D texture1;
uniform float time;

void main()
{
    vec3 sun = vec3(.5, .5, -.7);
    sun /= sqrt(sun.x * sun.x + sun.y * sun.y + sun.z * sun.z);

    float lamb_diff = max(dot(-sun, Normal), 0);

    vec3 view = View / sqrt(View.x * View.x + View.y * View.y + View.z * View.z);

    vec3 half_vec = sun + View;
    float specular = pow(dot(half_vec, Normal), 1.);

    FragColor = vec3(.0, .1, 1.) * lamb_diff + vec3(0.7, 0.7, 0.7) * specular;
}