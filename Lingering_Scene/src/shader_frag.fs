#version 330 core

layout(location = 0) out vec3 FragColor;

in vec2 TexCoord;
in vec3 FragCoord;
in vec3 Normal;

uniform sampler2D texture1;
uniform vec3 light_col;
uniform vec3 light_pos;
uniform vec3 view_pos;

void main()
{
    // lighting that is always there
    vec3 ambient = vec3(0.5);// * light_col;

    // light projected from the light source
    vec3 light_dir = normalize(light_pos - FragCoord);
    vec3 diffuse = max(dot(Normal, light_dir), 0.0) * light_col;

    // reflective lighting from light source
    float spec_strength = 1.0;
    float shininess = 128. * 2;
    vec3 view_dir = normalize(view_pos - FragCoord);
    vec3 reflect_dir = reflect(-light_dir, Normal);
    vec3 specular = spec_strength * light_col * pow(max(dot(view_dir, reflect_dir), 0.0), shininess);

    vec3 col = 0.2 + vec3(.2, .2, .5) * texture(texture1, TexCoord).xyz;
    col = texture(texture1, TexCoord).xyz;
    FragColor = (ambient + diffuse + specular) * col;
}