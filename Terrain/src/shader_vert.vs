#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 FragCoord;
out vec3 Normal;
flat out int Instance;

uniform mat4 model;
uniform mat4 view;
uniform mat4 perspective;
uniform mat3 norm_mat;

uniform vec2 offsets[2500];

float smoothstep(float x);
uint pcgHash(uint seed);
vec2 pcgHash(vec2 p);

void main()
{
    vec2 offset = offsets[gl_InstanceID];
    vec2 pos = (aPos.xy + 1) * .5;
    vec2 offs = (offset + pos) + 200;
    offs = vec2(202);
    vec2 uv = (offset + pos + 25) * 0.02;

    float grad1 = dot(pcgHash(offs), uv);
    float grad2 = dot(pcgHash(offs + vec2(1, 0)), vec2(uv.x - 1, uv.y));
    float grad3 = dot(pcgHash(offs + vec2(0, 1)), vec2(uv.x, uv.y - 1));
    float grad4 = dot(pcgHash(offs + 1), uv - 1);

    float bilinear = mix(mix(grad1, grad2, smoothstep(uv.x)), mix(grad3, grad4, smoothstep(uv.x)), smoothstep(uv.y));

    vec4 m_pos = model * vec4(aPos.xy + 2 * offset, aPos.z + 50 * bilinear, 1.0);

    FragCoord = vec3(m_pos);
    TexCoord = aTexCoord;
    Normal = normalize(norm_mat * aNormal);
    Instance = gl_InstanceID;

	gl_Position = perspective * view * m_pos;
}

vec2 pcgHash(vec2 p)
{
    uint x = pcgHash(uint(p.x) + pcgHash(uint(p.y)));
    return vec2(x, pcgHash(x)) / float(0xFFFFFFFFu) * 2 - 1;
}

uint pcgHash(uint seed) {
    seed = seed * 747796405u + 2891336453u;
    uint xor = ((seed >> 18u) ^ seed) >> 27u;
    uint rot = seed >> 22u;
    return (xor >> rot) | (xor << (32u - rot));
}

float smoothstep(float x)
{
    return x * x * x * (x * (x * 6 - 15) + 10);
}