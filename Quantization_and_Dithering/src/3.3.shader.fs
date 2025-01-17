#version 330 core

layout(location = 0) out vec3 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;

void main()
{
    float downscale = 2.;
    float dither_spread = 0.5;
    // This will affect all 3 colors, aka num_colors = 2 actually allows for 6 colors
    float num_colors = 5;
    //float sharpness = 1.;

    mat4 bayer;
    bayer[0] = vec4(0, 12, 3, 15);
    bayer[1] = vec4(8, 4, 11, 7);
    bayer[2] = vec4(2, 14, 1, 13);
    bayer[3] = vec4(10, 6, 9, 5);

    vec2 texture_size = textureSize(texture1, 0);
    vec2 quantized_coord = floor(TexCoord / downscale * texture_size) * downscale / texture_size;

    float dither_noise = (bayer[int(TexCoord.y * texture_size.y) % 4][int(TexCoord.x * texture_size.x) % 4] / 16.) - 0.5;

    vec3 color = texture(texture1, quantized_coord).xyz;

    //vec3 gray_scale = vec3(0.2989, 0.589, 0.114);
    //vec3 color = vec3(dot(gray_scale, texture(texture1, quantized_coord).xyz));

    /*vec3 color = texture(texture1, quantized_coord).xyz * sharpness * 5;

    color -= texture(texture1, quantized_coord + (downscale / texture_size.x)).xyz * sharpness;
    color -= texture(texture1, quantized_coord - (downscale / texture_size.x)).xyz * sharpness;
    color -= texture(texture1, quantized_coord + (downscale / texture_size.y)).xyz * sharpness;
    color -= texture(texture1, quantized_coord - (downscale / texture_size.y)).xyz * sharpness;*/

    color = floor(color * (num_colors - 1.) + 0.5) / (num_colors - 1.);

    FragColor = color + (dither_spread * dither_noise);
}