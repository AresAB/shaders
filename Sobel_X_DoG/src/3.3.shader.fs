#version 330 core

layout(location = 0) out vec3 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;

float Gaussian_2D(int x, int y, float sigma, int kernal_size)
{
    // equation to create a 2D gaussian weight
    // exp() is equivalent to e^(parameters)
    return (1 / (2 * 3.14 * sigma * sigma)) * exp(-1 * (pow(x - floor(kernal_size / 2.0), 2) + pow(y - floor(kernal_size / 2.0), 2)) / (2 * sigma * sigma));
}

float Gaussian_1D(int x, float sigma, int kernal_size)
{
    // equation to create a 1D gaussian weight
    // exp() is equivalent to e^(parameters)
    return (1 / (2 * 3.14 * sigma * sigma)) * exp(-1 * pow(x - floor(kernal_size / 2.0), 2) / (2 * sigma * sigma));
}

vec4 Tensor_Structure(mat3 mat_h, mat3 mat_v, vec2 tex_coord, vec2 tex_size, vec3 gray_scale)
{
    // creates a vec4 storing information relevant to finding the reverse gradient of a pixel
    // aka get data to eventually find direction of least change
    // this stuff is confusing, so go check out Acerola's video on the difference of gaussians for an explanation

    float sobel_x = 0;
    float sobel_y = 0;
    
    for (int row = 0; row < 3; row += 1) {
        for (int col = 0; col < 3; col += 1) {

            vec4 tex_color = texture(texture1, tex_coord + vec2((row - 1) * tex_size.x, (col - 1) * tex_size.y));
            float gray_scale_color = dot(tex_color.xyz, gray_scale);

            sobel_y += mat_v[col][row] * gray_scale_color;
            sobel_x += mat_h[col][row] * gray_scale_color;

        }
    }

    float E = pow(sobel_x, 2);
    float F = sobel_x * sobel_y;
    float G = pow(sobel_y, 2);

    return vec4(E, F, F, G);
}

void main()
{
    // not really a parameter
    int kernal_size = 5;
    // controls general image sharpness
    float tau = 100.;
    // controls the seperator of the threshold
    float epsilon = 0.6;
    // controls the steepness of the lower threshold, aka higher phi = sharper transition between black and white
    float phi = 7.;
    // controls strength of the tangent flow blur, aka higher sigma_c = stronger de-noising
    float sigma_c = 0.8;

    vec4 t_struct_sum = vec4(0);
    float G_sigma_c_sum = 0;

    vec2 tex_size = 1.0 / textureSize(texture1, 0);
    vec3 gray_scale = vec3(0.2989, 0.589, 0.114);

    mat3 edge_vert;
    edge_vert[0] = vec3(1, 0, -1);
    edge_vert[1] = vec3(2, 0, -2);
    edge_vert[2] = vec3(1, 0, -1);

    mat3 edge_horiz;
    edge_horiz[0] = vec3(1, 2, 1);
    edge_horiz[1] = vec3(0, 0, 0);
    edge_horiz[2] = vec3(-1, -2, -1);

    for (int row = 0; row < kernal_size; row += 1) {
        for (int col = 0; col < kernal_size; col += 1) {

            vec4 t_struct = Tensor_Structure(edge_horiz, edge_vert, TexCoord + vec2((row - 2) * tex_size.x, (col - 2) * tex_size.y), tex_size, gray_scale);
            
            float gaussian = Gaussian_2D(row, col, sigma_c, kernal_size);
            t_struct_sum += t_struct * gaussian;
            G_sigma_c_sum += gaussian;

        }
    }

    vec4 t_struct = t_struct_sum / G_sigma_c_sum;
    
    float eigen_val = ( t_struct.x + t_struct.w + sqrt( pow( t_struct.x - t_struct.w, 2 ) + ( 4 * pow( t_struct.y, 2 ) ) ) ) / 2.;
    vec2 eigen_vec = vec2(eigen_val - t_struct.x, -1 * t_struct.y);

    FragColor = vec3(atan(eigen_vec.y / eigen_vec.x));
}