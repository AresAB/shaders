#version 330 core

layout(location = 0) out vec3 FragColor;

in vec2 TexCoord;
in vec3 FragCoord;
in vec3 Normal;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct PointLight {
    vec3 pos;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float linear_decay;
    float exp_decay;
};

#define NR_POINT_LIGHTS 2

uniform vec3 view_pos;
uniform Material material;
uniform PointLight pointlights[NR_POINT_LIGHTS];

vec3 calc_point_light(PointLight light, vec3 diffuse_tex, vec3 specular_tex);

void main()
{
    vec3 diffuse_tex = texture(material.diffuse, TexCoord).xyz;
    vec3 specular_tex = texture(material.specular, TexCoord).xyz;
    vec3 light = vec3(0);
    for (int i=0; i < NR_POINT_LIGHTS; i++) {
        light += calc_point_light(pointlights[i], material.shininess, diffuse_tex, specular_tex);
    }

    FragColor = light;
}

vec3 calc_point_light(PointLight light, float shininess, vec3 diffuse_tex, vec3 specular_tex)
{
    // lighting that is always there
    vec3 ambient = diffuse_tex * light.ambient;

    // light projected from the light source
    vec3 light_dir = light.pos - FragCoord;
    float light_dist = sqrt(light_dir.x * light_dir.x + light_dir.y * light_dir.y + light_dir.z * light_dir.z);
    light_dir = normalize(light_dir);
    vec3 diffuse = max(dot(Normal, light_dir), 0.0) * diffuse_tex * light.diffuse;

    // reflective lighting from light source
    vec3 view_dir = normalize(view_pos - FragCoord);
    vec3 reflect_dir = reflect(-light_dir, Normal);
    vec3 specular = pow(max(dot(view_dir, reflect_dir), 0.0), shininess) * specular_tex * light.specular;

    // original OpenGL lighting model equation for realistic light falloff
    // technically the "1" is another constant you can tune, no one does
    float attenuation = 1 + light.linear_decay * light_dist + light.exp_decay * light_dist * light_dist;

    return (ambient + diffuse + specular) / attenuation;
}