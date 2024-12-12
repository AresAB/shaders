#version 330 core

out vec4 FragColor;
  
in vec2 TexCoord;

uniform sampler2D texture1;

float G(int x, int y, float sigma, int kernal_size)
{
    // equation to create a gaussian weight
    // exp() is equivalent to e^(parameters)
    return (1 / (2 * 3.14 * sigma * sigma)) * exp(-1 * (pow(x - floor(kernal_size / 2.0), 2) + pow(y - floor(kernal_size / 2.0), 2)) / (2 * sigma * sigma));
}

// proof of concept, don't use cause no reason to iterate twice
/*mat4 G_kernal(float sigma)
{
    int kernal_size = 3;
    mat3 kernal;
    for (int r = 0; r < kernal_size; r += 1) {
        for (int c = 0; c < kernal_size; c += 1) {
            kernal[r][c] = G(r - 2, 2 - c, sigma);
        }
    }
}*/

void main()
{
    int kernal_size = 5;
    float sigma = 1.0;
    vec2 tex_size = 1.0 / textureSize(texture1, 0);
    vec4 color_sum = vec4(0);
    float G_sum = 0;

    for (int row = 0; row < kernal_size; row += 1) {
        for (int col = 0; col < kernal_size; col += 1) {
            vec4 tex_color = texture(texture1, TexCoord + vec2((row - 1) * tex_size.x, (col - 1) * tex_size.y));
            float gaussian = G(row, col, sigma, kernal_size);
            G_sum += gaussian;
            tex_color.xyz *= gaussian;
            color_sum += tex_color;
        }
    }

    color_sum.xyz /= G_sum;
    FragColor = color_sum;
}