#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 FragCoord;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 perspective;
uniform mat3 norm_mat;

void main()
{
    FragCoord = vec3(model * vec4(aPos, 1.0));
    TexCoord = aTexCoord;
    Normal = normalize(norm_mat * aNormal);

	gl_Position = perspective * view * model * vec4(aPos, 1.0);
}