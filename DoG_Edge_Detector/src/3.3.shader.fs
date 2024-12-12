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

void main()
{
    int kernal_size = 5;
    vec2 tex_size = 1.0 / textureSize(texture1, 0);
    vec3 gray_scale = vec3(0.2989, 0.589, 0.114);

    float sigma1 = 1.0;
    float color_sum1 = 0;
    float G_sum1 = 0;

    float sigma2 = sigma1 * 1.6;
    float color_sum2 = 0;
    float G_sum2 = 0;

    for (int row = 0; row < kernal_size; row += 1) {
        for (int col = 0; col < kernal_size; col += 1) {

            vec4 tex_color = texture(texture1, TexCoord + vec2((row - 1) * tex_size.x, (col - 1) * tex_size.y));
            // grayscale the color to desaturate for better DoG
            float gray_scale_color = dot(tex_color.xyz, gray_scale);

            float gaussian1 = G(row, col, sigma1, kernal_size);
            G_sum1 += gaussian1;
            color_sum1 += gray_scale_color * gaussian1;

            float gaussian2 = G(row, col, sigma2, kernal_size);
            G_sum2 += gaussian2;
            color_sum2 += gray_scale_color * gaussian2;
        }
    }

    color_sum1 /= G_sum1;
    color_sum2 /= G_sum2;
    
    vec4 color = vec4(1);
    // Step will return 0.0 unless the value is over 0.01,
    // in that case it will return 1.0
    color.xyz = vec3(step(0.01, color_sum2 - color_sum1));

    FragColor = color;
}