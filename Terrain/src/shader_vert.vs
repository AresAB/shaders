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

float quint_smoothstep(float x);
float quint_smoothstep(float x, float y, float a);
uint pcgHash(uint seed);
vec2 pcgHash(vec2 p);
float perlin(vec2 uv, vec2 seed);
vec3 perlin_gradient(vec2 uv, vec2 seed);
float fbm (vec2 uv, int depth, vec2 seed);
vec3 fbm_normal (vec2 uv, int depth, vec2 seed);
float fbm_sculpted (vec2 uv, vec2 seed);
vec3 fbm_sculpted_normal (vec2 uv, vec2 seed);
float quint_smoothstep_deriv(float x, float y, float a);
float quint_smoothstep_deriv(float x);
vec4 fbm_struct (vec2 uv, int depth, vec2 seed);

void main()
{
    vec2 offset = vec2(gl_InstanceID % size_len, gl_InstanceID / size_len);
    vec2 seed = vec2(230);
    vec2 uv = offset + (aPos.xy + 1) * .5;

    int lat_size = size_len / lattices;
    seed += floor(uv / lat_size);
    uv = vec2(float(int(uv.x) % lat_size) / lat_size, float(int(uv.y) % lat_size) / lat_size);

    //float noise = fbm(uv, 10, seed); // min of depth 2, use perlin() for depth 1
    //float noise = fbm_sculpted(uv, seed);
    vec4 noise_struct = fbm_struct(uv, 10, seed);
    float noise = noise_struct.x;
    //float noise = perlin(uv, seed);

    // the smoothstep just lifts up any terrain above .2
    float final = 20 * (noise + 1 * smoothstep(.2, 1, noise));
    vec4 m_pos = model * vec4(aPos.xy + 2 * (offset - size_len * .5), aPos.z + final, 1.0);

    FragCoord = vec3(m_pos);
    TexCoord = aTexCoord;
    //Normal = normalize(norm_mat * aNormal);
    //Normal = fbm_normal(uv, 10, seed);
    //Normal = fbm_sculpted_normal(uv, seed);
    Normal = noise_struct.yzw;
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

vec3 fbm_normal (vec2 uv, int depth, vec2 seed) // calculate normal through cross product-ing the gradient (partial x crossed with partial y)
{
    // I'm not sure how to get the partial derivative of perlin or fbm noise, so I just estimate the slope
    float x_partial = (fbm(uv, depth, seed + vec2(1, 0)) - fbm(uv, depth, seed - vec2(1, 0))) * 0.5;
    float z_partial = (fbm(uv, depth, seed + vec2(0, 1)) - fbm(uv, depth, seed - vec2(0, 1))) * 0.5;
    return normalize(cross(normalize(vec3(0, z_partial, 1)), normalize(vec3(1, x_partial, 0))));
}

vec4 fbm_struct (vec2 uv, int depth, vec2 seed)
{
    mat2 pyth_trip = mat2(.8, .6, -.6, .8);
    vec2 p = 2 * pyth_trip * (uv + seed);
    float scale = .5;
    vec3 noise = perlin_gradient(uv, seed) + perlin_gradient(fract(p), floor(p)) * scale;
    for (int i=2; i < depth; i++)
    {
        scale *= .5;
        p = 2 * pyth_trip * p;
        noise += perlin_gradient(fract(p), floor(p)) * scale;
    }
    float result = noise.x;
    noise = normalize(cross(vec3(0., noise.z, 1.), vec3(1., noise.y, 0.)));
    return vec4(result, noise.x, noise.y, noise.z);
}

float fbm_sculpted (vec2 uv, vec2 seed) // fractional brownian motion
{
    mat2 pyth_trip = mat2(.8, .6, -.6, .8);
    vec2 p = 2 * pyth_trip * (uv + seed);
    float scale = .5;
    float noise = perlin(uv, seed) + perlin(fract(p), floor(p)) * scale;
    for (int i=2; i < 4; i++)
    {
        scale *= .5;
        p = 2 * pyth_trip * p;
        noise += perlin(fract(p), floor(p)) * scale;
    }
    for (int i=4; i < 7; i++)
    {
        scale *= .5;
        p = 2 * pyth_trip * p;
    }
    for (int i=7; i < 10; i++)
    {
        scale *= .5;
        p = 2 * pyth_trip * p;
        noise += perlin(fract(p), floor(p)) * scale;
    }
    return noise;
}

vec3 fbm_sculpted_normal (vec2 uv, vec2 seed)
{
    float x_partial = (fbm_sculpted(uv, seed + vec2(1, 0)) - fbm_sculpted(uv, seed - vec2(1, 0))) * 0.5;
    float z_partial = (fbm_sculpted(uv, seed + vec2(0, 1)) - fbm_sculpted(uv, seed - vec2(0, 1))) * 0.5;
    return normalize(cross(normalize(vec3(0, z_partial, 1)), normalize(vec3(1, x_partial, 0))));
}

float perlin(vec2 uv, vec2 seed) 
{
    float grad1 = dot(pcgHash(seed), uv);
    float grad2 = dot(pcgHash(seed + vec2(1, 0)), vec2(uv.x - 1, uv.y));
    float grad3 = dot(pcgHash(seed + vec2(0, 1)), vec2(uv.x, uv.y - 1));
    float grad4 = dot(pcgHash(seed + 1), uv - 1);

    return mix(mix(grad1, grad2, quint_smoothstep(uv.x)), mix(grad3, grad4, quint_smoothstep(uv.x)), quint_smoothstep(uv.y));
}

vec3 perlin_gradient(vec2 uv, vec2 seed) 
{
    float grad1 = dot(pcgHash(seed), uv);
    float grad2 = dot(pcgHash(seed + vec2(1, 0)), vec2(uv.x - 1, uv.y));
    float grad3 = dot(pcgHash(seed + vec2(0, 1)), vec2(uv.x, uv.y - 1));
    float grad4 = dot(pcgHash(seed + 1), uv - 1);
    
    vec3 result;
    float linear1 = mix(grad1, grad2, quint_smoothstep(uv.x));
    float linear2 = mix(grad3, grad4, quint_smoothstep(uv.x));
    result.x = mix(linear1, linear2, quint_smoothstep(uv.y));
    result.y = (1 - quint_smoothstep(uv.x)) * quint_smoothstep_deriv(uv.x);
    result.y *= result.y * 2;
    result.z = quint_smoothstep_deriv(uv.y) * (linear2 - linear1);
    return result;
}

vec2 pcgHash(vec2 p)
{
    uint x = pcgHash(uint(p.x) + pcgHash(uint(p.y)));
    return vec2(cos(x), sin(x));
}

uint pcgHash(uint seed) 
{
    seed = seed * 747796405u + 2891336453u;
    uint xor = ((seed >> 18u) ^ seed) >> 27u;
    uint rot = seed >> 22u;
    return (xor >> rot) | (xor << (32u - rot));
}

float quint_smoothstep(float x, float y, float a)
{
    return quint_smoothstep(clamp((a - x) / (y - x), 0.0, 1.0));
}

float quint_smoothstep(float x)
{
    return x * x * x * (x * (x * 6 - 15) + 10);
}

float quint_smoothstep_deriv(float x, float y, float a)
{
    return quint_smoothstep_deriv(clamp((a - x) / (y - x), 0.0, 1.0));
}

float quint_smoothstep_deriv(float x)
{
    return x * x * (x * (x * 30 - 60) + 30);
}

