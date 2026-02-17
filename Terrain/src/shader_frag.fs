#version 330 core

layout(location = 0) out vec3 FragColor;

in vec2 TexCoord;
in vec3 FragCoord;
in vec3 Normal;
in float Height;

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
    float height = floor(Height * 50) / 50;
    height = Height;
    vec3 normal = normalize(Normal);
    //FragColor = height + vec3(0.7, 0.4, 0.2);
    //FragColor = height + vec3(0.7, 0.4, 0.2);
    //vec3 color = vec3(0.7, 0.4, 0.2);
    vec3 color = mix(vec3(0., 0.5, 0.), vec3(0.7, 0.4, 0.2), vec3(smoothstep(0.6, 0.7, normal.y)));
    //color += height * vec3(0., 0., 0.5);
    color *= calcDirLight(material, light);
    FragColor = color;
}

vec3 calcDirLight(Material mat, DirLight light)
{
    // lighting that is always there
    vec3 ambient = light.ambient;
    vec3 normal = normalize(Normal);
    // light projected from the light source
    vec3 light_dir = normalize(-light.dir);
    vec3 diffuse = max(dot(normal, light_dir), 0.0) * light.diffuse;

    // reflective lighting from light source
    vec3 view_dir = normalize(view_pos - FragCoord);
    vec3 reflect_dir = reflect(-light_dir, normal);
    vec3 specular = pow(max(dot(view_dir, reflect_dir), 0.0), mat.shininess) * light.specular;

    return ambient + diffuse + specular;
}
