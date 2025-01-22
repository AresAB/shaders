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

    return rgb;
}

void main()
{
    float num_colors = 5;
    float luminance = 0.3;
    float chroma = 0.2;
    float hue = 0.;
    // remember to keep one of these spreads constant for the sake of color harmony
    float l_spread = 0.2;
    float c_spread = 0.;
    float h_spread = 0.5;

    l_spread *= (5./num_colors);
    c_spread *= (5./num_colors);
    h_spread *= (5./num_colors);

    vec3 gray_scale = vec3(0.2989, 0.589, 0.114);

    float uv = dot(gray_scale, texture(texture1, TexCoord).xyz);
    uv = floor(uv * (num_colors - 1.) + 0.5) / (num_colors - 1.);

    vec3 oklch_col;
    oklch_col.x = luminance + (l_spread * uv * (num_colors - 1.));
    oklch_col.y = chroma + (c_spread * uv * (num_colors - 1.));
    oklch_col.z = hue + (h_spread * uv * (num_colors - 1.));

    FragColor = oklch_to_RGB(oklch_col);
}