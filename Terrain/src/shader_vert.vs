#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 FragCoord;
out vec3 Normal;
out float Height;

uniform mat4 model;
uniform mat4 view;
uniform mat4 perspective;
uniform mat3 norm_mat;

uniform int lattices;
uniform int size_len;

float smoothstep(float x);
uint pcgHash(uint seed);
vec2 pcgHash(vec2 p);
float perlin(vec2 uv, vec2 seed);
float fbm (vec2 uv, int depth, vec2 seed);

void main()
{
    vec2 offset = vec2(gl_InstanceID % size_len, gl_InstanceID / size_len);
    vec2 seed = vec2(230);
    vec2 uv = offset + (aPos.xy + 1) * .5;

    int lat_size = size_len / lattices;
    seed += floor(uv / lat_size);
    uv = vec2(float(int(uv.x) % lat_size) / lat_size, float(int(uv.y) % lat_size) / lat_size);

    float noise = fbm(uv, 10, seed); // min of depth 2, use perlin() for depth 1
    //float noise = perlin(uv, seed);
    vec4 m_pos = model * vec4(aPos.xy + 2 * (offset - size_len * .5), aPos.z + 20 * noise, 1.0);

    FragCoord = vec3(m_pos);
    TexCoord = aTexCoord;
    Normal = normalize(norm_mat * aNormal);
    Height = noise;

	gl_Position = perspective * view * m_pos;
}

float fbm (vec2 uv, int depth, vec2 seed) // fractional brownian motion
{
    mat2 pyth_trip = mat2(.8, .6, -.6, .8);
    vec2 p = 2 * pyth_trip * (uv + seed);
    float scale = .5;
    float noise = perlin(uv, seed) + perlin(fract(p), floor(p)) * scale;
    for (int i=2; i < depth; i++)
    {
        scale *= .5;
        p = 2 * pyth_trip * p;
        noise += perlin(fract(p), floor(p)) * scale;
    }
    return noise;
}

/*float fbm (vec2 uv, int depth, vec2 seed)
{
    mat2 pyth_trip = mat2(4./5, 3./5, -3./5, 4./5);
    mat2 rot;
    float noise = perlin(uv, seed);
    for (int i=1; i < depth; i++)
    {
        rot = pyth_trip;
        for (int j=1; j < i; j++)
        {
            rot *= pyth_trip;
        }
        vec2 p = pow(2, i) * (rot * uv);
        noise += perlin(fract(p), seed + floor(p)) / pow(2, i);
    }
    return noise;
}*/

float perlin(vec2 uv, vec2 seed) 
{
    float grad1 = dot(pcgHash(seed), uv);
    float grad2 = dot(pcgHash(seed + vec2(1, 0)), vec2(uv.x - 1, uv.y));
    float grad3 = dot(pcgHash(seed + vec2(0, 1)), vec2(uv.x, uv.y - 1));
    float grad4 = dot(pcgHash(seed + 1), uv - 1);

    return mix(mix(grad1, grad2, smoothstep(uv.x)), mix(grad3, grad4, smoothstep(uv.x)), smoothstep(uv.y));
}

vec2 pcgHash(vec2 p)
{
    uint x = pcgHash(uint(p.x) + pcgHash(uint(p.y)));
    return vec2(cos(x), sin(x));
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