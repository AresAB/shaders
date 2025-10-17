#version 330 core

layout(location = 0) out vec3 FragColor;

in vec2 TexCoord;
in vec3 FragCoord;
in vec3 Normal;
flat in int Instance;

struct Material {
    float shininess;
};

struct DirLight {
    vec3 dir;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform vec3 view_pos;
uniform Material material;
uniform DirLight light;

uniform vec2 offsets[2500];

vec3 calcDirLight(Material mat, DirLight light);
float smoothstep(float x);
uint pcgHash(uint seed);
vec2 pcgHash(vec2 p);

void main()
{
    //FragColor = calcDirLight(material, light);
    vec2 offs = offsets[Instance] + 200;
    offs = vec2(201, 201);
    vec2 coord = (offsets[Instance] + TexCoord + 25) / 50;

    float grad1 = dot(pcgHash(offs), coord);
    float grad2 = dot(pcgHash(offs + vec2(1, 0)), vec2(coord.x - 1, coord.y));
    float grad3 = dot(pcgHash(offs + vec2(0, 1)), vec2(coord.x, coord.y - 1));
    float grad4 = dot(pcgHash(offs + 1), coord.xy - 1);

    float bilinear = mix(mix(grad1, grad2, smoothstep(coord.x)), mix(grad3, grad4, smoothstep(coord.x)), smoothstep(coord.y));

    FragColor = vec3(bilinear);
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

vec3 calcDirLight(Material mat, DirLight light)
{
    // lighting that is always there
    vec3 ambient = light.ambient;

    // light projected from the light source
    vec3 light_dir = normalize(-light.dir);
    vec3 diffuse = max(dot(Normal, light_dir), 0.0) * light.diffuse;

    // reflective lighting from light source
    vec3 view_dir = normalize(view_pos - FragCoord);
    vec3 reflect_dir = reflect(-light_dir, Normal);
    vec3 specular = pow(max(dot(view_dir, reflect_dir), 0.0), mat.shininess) * light.specular;

    return ambient + diffuse + specular;
}