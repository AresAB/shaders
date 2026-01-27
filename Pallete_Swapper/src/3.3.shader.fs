#version 330 core

layout(location = 0) out vec3 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;

vec3 oklch_to_RGB(vec3 LCH)
{
    // convert to oklab color space
    float L = LCH.x;
    float a = LCH.y * cos(LCH.z);
    float b = LCH.y * sin(LCH.z);

    // convert to lms color space
    float l = pow(L + a * 0.3963377774 + b * 0.2158037573, 3);
    float m = pow(L + a * -0.1055613458 + b * -0.0638541728, 3);
    float s = pow(L + a * -0.0894841775 + b * -1.2914855480, 3);

    vec3 rgb;

    rgb.r = l * 4.0767416621 + m * -3.3077115913 + s * 0.2309699292;
    rgb.g = l * -1.2684380046 + m * 2.6097574011 + s * -0.3413193965;
    rgb.b = l * -0.0041960863 + m * -0.7034186147 + s * 1.7076147010;

    rgb = max(min(rgb, 1.), 0);

    return rgb;
}

/*vec3 rgb_to_oklch(vec3 rgb) {
    float r_ = rgb.r;
    float g_ = rgb.g;
    float b_ = rgb.b;

    float l = 0.4122214708 * r_ + 0.5363325363 * g_ + 0.0514459929 * b_;
    float m = 0.2119034982 * r_ + 0.6806995451 * g_ + 0.1073969566 * b_;
    float s = 0.0883024619 * r_ + 0.2817188376 * g_ + 0.6299787005 * b_;

    float l_ = pow(l, 0.33);
    float m_ = pow(m, 0.33);
    float s_ = pow(s, 0.33);

    float L = 0.2104542553 * l_ + 0.7936177850 * m_ - 0.0040720468 * s_;
    float a = 1.9779984951 * l_ - 2.4285922050 * m_ + 0.4505937099 * s_;
    float b = 0.0259040371 * l_ + 0.7827717662 * m_ - 0.8086757660 * s_;

    float C = pow(a * a + b * b, 0.5);
    float h = atan(b, a);

    return vec3(round(L * 100) * 0.01, round(C * 100) * 0.01, round(h * 100) * 0.01);
}*/

void main()
{
    float num_colors = 8;
    float luminance = 0.4;
    float chroma = 0.3;
    float hue = 15; // 0 - 100
    // remember to keep one of these spreads constant for the sake of color harmony
    float l_spread = 0.07;
    float c_spread = 0.;
    float h_spread = 1.; // 0 - 100

    float dither_spread = .5;

    mat4 bayer;
    bayer[0] = vec4(0, 12, 3, 15);
    bayer[1] = vec4(8, 4, 11, 7);
    bayer[2] = vec4(2, 14, 1, 13);
    bayer[3] = vec4(10, 6, 9, 5);
    vec2 texture_size = textureSize(texture1, 0);
    float dither_noise = (bayer[int(TexCoord.y * texture_size.y) % 4][int(TexCoord.x * texture_size.x) % 4] / 16.) - 0.5;

    vec3 gray_scale = vec3(0.2989, 0.589, 0.114);

    float uv = dot(gray_scale, texture(texture1, TexCoord).xyz);
    uv = floor(uv * (num_colors - 1.) + 0.5) / (num_colors - 1.);

    vec3 oklch_col;
    oklch_col.x = luminance + (l_spread * uv * (num_colors - 1.));
    oklch_col.y = chroma + (c_spread * uv * (num_colors - 1.));
    oklch_col.z = hue + (h_spread * uv * (num_colors - 1.));

    oklch_col.x = max(min(oklch_col.x, 1.), 0);
    oklch_col.y = max(min(oklch_col.y, 1.), 0);
    oklch_col.z *= 3.6;
    oklch_col.z = int(oklch_col.z + 360) % (360);
    oklch_col.z *= 3.14 / 180.;

    FragColor = oklch_to_RGB(oklch_col) + (dither_spread * dither_noise);
}