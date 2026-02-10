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

struct Spotlight {
    vec3 pos;
    vec3 dir;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float umbra;
    float penumbra;
    float linear_decay;
    float exp_decay;
};

#define NR_POINT_LIGHTS 2
#define NR_SPOTLIGHTS 1

uniform vec3 view_pos;
uniform Material material;
uniform PointLight pointlights[NR_POINT_LIGHTS];
uniform Spotlight spotlights[NR_SPOTLIGHTS];

vec3 calc_point_light(PointLight light, float shininess, vec3 diffuse_tex, vec3 specular_tex);
vec3 calc_spotlight(Spotlight light, float shininess, vec3 diffuse_tex, vec3 specular_tex);

void main()
{
    vec3 diffuse_tex = texture(material.diffuse, TexCoord).xyz;
    vec3 specular_tex = texture(material.specular, TexCoord).xyz;
    vec3 light = vec3(0);
    for (int i=0; i < NR_POINT_LIGHTS; i++) {
        light += calc_point_light(pointlights[i], material.shininess, diffuse_tex, specular_tex);
    }
    for (int i=0; i < NR_SPOTLIGHTS; i++) {
        light += calc_spotlight(spotlights[i], material.shininess, diffuse_tex, specular_tex);
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

vec3 calc_spotlight(Spotlight light, float shininess, vec3 diffuse_tex, vec3 specular_tex)
{
    // lighting that is always there
    vec3 ambient = diffuse_tex * light.ambient;

    // light projected from the light source
    vec3 diffuse = max(dot(Normal, -light.dir), 0.0) * diffuse_tex * light.diffuse;

    // reflective lighting from light source
    vec3 view_dir = normalize(view_pos - FragCoord);
    vec3 reflect_dir = reflect(light.dir, Normal);
    vec3 specular = pow(max(dot(view_dir, reflect_dir), 0.0), shininess) * specular_tex * light.specular;


    vec3 light_dist_vec = FragCoord - light.pos;
    float light_dist = sqrt(light_dist_vec.x * light_dist_vec.x + light_dist_vec.y * light_dist_vec.y + light_dist_vec.z * light_dist_vec.z);
    // original OpenGL lighting model equation for realistic light falloff
    // technically the "1" is another constant you can tune, no one does
    float attenuation = 1 + light.linear_decay * light_dist + light.exp_decay * light_dist * light_dist;

    vec3 col = (ambient + diffuse + specular) / attenuation;
    float dir = clamp((dot(normalize(light_dist_vec), light.dir) - light.umbra)/(light.penumbra - light.umbra), 0.f, 1.f);
    dir = dir * dir * (3 - 2 * dir); // smoothstep
    return col * dir;
}
