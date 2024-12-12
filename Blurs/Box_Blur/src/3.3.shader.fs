#version 330 core

out vec4 FragColor;
  
in vec2 TexCoord;

uniform sampler2D texture1;

void main()
{
    int kernal_size = 5; //always odd
    vec2 tex_size = 1.0 / textureSize(texture1, 0);
    vec4 color_sum = vec4(0);

    for (int row = 0; row < kernal_size; row += 1) {
        for (int col = 0; col < kernal_size; col += 1) {
            color_sum += texture(texture1, TexCoord + vec2((row - 1) * tex_size.x, (col - 1) * tex_size.y));
        }
    }

    color_sum.xyz /= kernal_size * kernal_size;
    FragColor = color_sum;
}