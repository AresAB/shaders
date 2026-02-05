#version 330 core

layout(location = 0) out vec3 FragColor;

in vec2 vTexCoord;
in vec3 vNormal;
in vec3 vPos;

uniform sampler2D texture1;
uniform vec3 uCoolColor;
uniform vec3 uWarmColor;
uniform vec3 uSurfaceMat;
uniform vec3 uLightPos;
uniform vec3 uLightColor;
uniform vec3 uViewPos;

vec3 lit(vec3 light_vec, vec3 normal, vec3 view_vec) {
    vec3 reflected = reflect(-light_vec, normal);
    float lerp = clamp(100. * dot(reflected, view_vec) - 97., 0., 1.);
    return mix(uWarmColor, vec3(2.), lerp);
}

void main() {
    vec3 cool = uCoolColor + 0.25 * uSurfaceMat;
    vec3 warm = uWarmColor + 0.25 * uSurfaceMat;

    vec3 normal = normalize(vNormal);
    vec3 view_vec = normalize(uViewPos - vPos);

    FragColor = 0.5 * uCoolColor;
    vec3 light_vec = normalize(uLightPos - vPos);
    float strength = clamp(dot(normal, light_vec), 0., 1.);
    FragColor += strength * uLightColor * lit(light_vec, normal, view_vec);
}
