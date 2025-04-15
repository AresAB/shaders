#version 330 core

layout(location = 0) out vec3 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;

vec3 Narkowicz_ACES(vec3 hdr)
{
    return clamp((hdr * (2.51 * hdr + 0.03)) / (hdr * (2.43 * hdr + 0.59) + 0.14), 0., 1.);
}

vec3 WhiteBalance(vec3 col, float te, float ti)
{
    float t1 = te * 10. / 6.;
    float t2 = ti * 10. / 6.;

    float x = 0.31271 - t1 * (t1 < 0 ? 0.1 : 0.05);
    float standIlluminantY = 2.87 * x - 3 * x * x - 0.27509507;
    float y = standIlluminantY + t2 * 0.05;

    vec3 w1 = vec3(0.949237, 1.03542, 1.08728);

    float Y = 1;
    float X = Y * x / y;
    float Z = Y * (1 - x - y) / y;
    float L = 0.7328 * X + 0.4296 * Y - 0.1624 * Z;
    float M = -0.7036 * X + 1.6975 * Y + 0.0061 * Z;
    float S = 0.003 * X + 0.0136 * Y + 0.9834 * Z;
    vec3 w2 = vec3(L, M, S);

    vec3 balance = vec3(w1.x / w2.x, w1.y / w2.y, w1.z / w2.z);

    mat3 LIN_2_LMS_MAT;
    LIN_2_LMS_MAT[0] = vec3(3.90405e-1, 7.08416e-2, 2.31082e-2);
    LIN_2_LMS_MAT[1] = vec3(5.49941e-1, 9.63172e-1, 1.35775e-3);
    LIN_2_LMS_MAT[2] = vec3(8.92632e-3, 1.35775e-3, 9.36245e-1);

    mat3 LMS_2_LIN_MAT;
    LMS_2_LIN_MAT[0] = vec3(2.85847e+0, -2.10182e-1, -4.1812e-2);
    LMS_2_LIN_MAT[1] = vec3(-1.62879e+0, 1.1582e+0, -1.18169e-1);
    LMS_2_LIN_MAT[2] = vec3(-2.4891e-2, 3.24281e-4, 1.06867e+0);

    vec3 lms = LIN_2_LMS_MAT * col;
    lms *= balance;
    return LMS_2_LIN_MAT * lms;
}

void main()
{
    // default is 1
    float exposure = 1.;
    // default is 0
    float temperature = -0.3;
    // default is 0
    float tint = 0.;
    // default is 1
    float contrast = 1;
    // default is 0
    float brightness = 0.;
    // default is (1., 1., 1.)
    vec3 color_filter = vec3(1., 1., 1.);
    // default is 1
    float saturation = 2.;
    // default is 1
    float gamma = 1;

    vec3 grayscale = vec3(0.2989, 0.589, 0.114);

    vec3 color = texture(texture1, TexCoord).xyz;
    
    color = max(color * exposure, 0.);

    color = WhiteBalance(color, temperature, tint);

    color = max(contrast * (color - 0.5) + 0.5 + brightness, 0.);

    color = max(color * color_filter, 0.);

    color = max(mix(vec3(dot(color, grayscale)), color, saturation), 0.);

    // tone mapping
    color = Narkowicz_ACES(color);

    color.x = pow(color.x, gamma);
    color.y = pow(color.y, gamma);
    color.z = pow(color.z, gamma);

    FragColor = color;
}