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

void main()
{
    float num_colors = 10;
    float divider = 5. / num_colors;

    float luminance_1 = 0.5;
    float chroma_1 = 0.3;
    float hue_1 = 215.;
    // remember to keep one of these spreads constant for the sake of color harmony
    float l_spread_1 = -0.1;
    float c_spread_1 = 0.;
    float h_spread_1 = 125;

    float luminance_2 = 0.64;
    float chroma_2 = 0.23;
    float hue_2 = 114.;
    // remember to keep one of these spreads constant for the sake of color harmony
    float l_spread_2 = 0.1;
    float c_spread_2 = 0.;
    float h_spread_2 = -57;

    vec3 gray_scale = vec3(0.2989, 0.589, 0.114);

    float uv = dot(gray_scale, texture(texture1, TexCoord).xyz);
    uv = floor(uv * (num_colors - 1.) + 0.5) / (num_colors - 1.);

    float is_pal_1 = floor(uv + divider);

    vec3 oklch_col;
    oklch_col.x = (luminance_1 + (l_spread_1 * uv)) * is_pal_1 + (luminance_2 + (l_spread_2 * uv)) * (1 - is_pal_1);
    oklch_col.y = (chroma_1 + (c_spread_1 * uv)) * is_pal_1 + (chroma_2 + (c_spread_2 * uv) * (1 - is_pal_1));
    oklch_col.z = (hue_1 + (h_spread_1 * uv)) * is_pal_1 + (hue_2 + (h_spread_2 * uv)) * (1 - is_pal_1);


    oklch_col.x = max(min(oklch_col.x, 1.), 0);
    oklch_col.y = max(min(oklch_col.y, 1.), 0);
    oklch_col.z = int(oklch_col.z + 360) % (360);
    oklch_col.z *= 3.14 / 180.;

    FragColor = oklch_to_RGB(oklch_col);
}