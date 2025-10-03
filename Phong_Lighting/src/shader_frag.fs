#version 330 core

layout(location = 0) out vec3 FragColor;

in vec2 TexCoord;
in vec3 FragCoord;
in vec3 Normal;

struct Material {
    float shininess;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
};

struct Light {
    vec3 pos;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform sampler2D texture1;
uniform vec3 view_pos;
uniform Material material;
uniform Light light;

void main()
{
    // lighting that is always there
    vec3 ambient = material.ambient * light.ambient;

    // light projected from the light source
    vec3 light_dir = normalize(light.pos - FragCoord);
    vec3 diffuse = max(dot(Normal, light_dir), 0.0) * material.diffuse * light.diffuse;

    // reflective lighting from light source
    vec3 view_dir = normalize(view_pos - FragCoord);
    vec3 reflect_dir = reflect(-light_dir, Normal);
    vec3 specular = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess) * material.specular * light.specular;

    //vec3 col = 0.2 + vec3(.2, .2, .5) * texture(texture1, TexCoord).xyz;
    vec3 col = texture(texture1, TexCoord).xyz;
    FragColor = (ambient + diffuse + specular) * col;
}