#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 vTexCoord;
out vec3 vNormal;
out vec3 vPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 perspective;

void main()
{
	vec4 worldPos = model * vec4(aPos, 1.0);
	vPos = worldPos.xyz;
	gl_Position = perspective * view * worldPos;
	vTexCoord = aTexCoord;
	vNormal = (model * vec4(aNormal, 1.0)).xyz;
}
