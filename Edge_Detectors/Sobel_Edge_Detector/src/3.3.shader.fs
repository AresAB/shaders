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

    float sobel_magnitude = sqrt(pow(sobel_x, 2) + pow(sobel_y, 2));
    float sobel_dir = atan(sobel_y / sobel_x);

    // This is for finding the tangent flow for direction of least change, relevant info if used for other effects
    // -------------------------------

    /*float E = pow(sobel_x, 2);
    float F = sobel_x * sobel_y;
    float G = pow(sobel_y, 2);

    float eigen_val = ( E + G + sqrt( pow( E - G, 2 ) + ( 4 * pow( F, 2 ) ) ) ) / 2.;
    vec2 eigen_vec = vec2(eigen_val - E, -1 * F);*/

    // -------------------------------

    FragColor = vec3(sobel_magnitude);
}