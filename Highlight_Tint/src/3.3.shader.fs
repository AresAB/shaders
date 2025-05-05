#version 330 core

layout(location = 0) out vec3 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;

void main()
{
    int kernal_size = 3;

    vec2 tex_size = 1.0 / textureSize(texture1, 0);
    vec3 gray_scale = vec3(0.2989, 0.589, 0.114);

    mat3 edge_vert;
    edge_vert[0] = vec3(1, 0, -1);
    edge_vert[1] = vec3(2, 0, -2);
    edge_vert[2] = vec3(1, 0, -1);

    float sobel_y = 0.;

    mat3 edge_horiz;
    edge_horiz[0] = vec3(1, 2, 1);
    edge_horiz[1] = vec3(0, 0, 0);
    edge_horiz[2] = vec3(-1, -2, -1);

    float sobel_x = 0.;

    for (int row = 0; row < kernal_size; row += 1) {
        for (int col = 0; col < kernal_size; col += 1) {

            vec4 tex_color = texture(texture1, TexCoord + vec2((row - 1) * tex_size.x, (col - 1) * tex_size.y));
            float gray_scale_color = dot(tex_color.xyz, gray_scale);

            sobel_y += edge_vert[col][row] * gray_scale_color;
            sobel_x += edge_horiz[col][row] * gray_scale_color;

        }
    }

    float sobel_mag = sqrt(pow(sobel_x, 2) + pow(sobel_y, 2));
    vec3 color = texture(texture1, TexCoord).xyz;
    float lumin = dot(color, gray_scale);
    float lumin_scale = floor(floor(lumin * 2. + 0.5) / 2.);

    vec3 highlight = vec3(1., 0.07, 0.72);
    vec3 col = mix(color, color + highlight * lumin, lumin_scale);
    col += highlight * sobel_mag * lumin;

    FragColor = col;
}
