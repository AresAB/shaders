#version 330 core

layout(location = 0) out vec3 FragColor;

in vec2 TexCoord;

float mandel(vec2 z, vec2 c, int deg, int strength) 
{
    vec2 result = z;
    for(int i=0; i < strength; i++) {
        vec2 old_result = result;
        for(int j=1; j < deg; j++)
        {
            result = vec2(result.x * old_result.x - result.y * old_result.y, old_result.y * result.x + result.y * old_result.x);
        }
        result += c;
        if(result.x > 50) 
        {
            float m = float(i) / strength;
            return (m - 2) * m + 1;
        }
    }
    return 0.;
}

void main()
{
    float zoom = 1.4;
    int strength = 50;
    float x_off = 0;

    vec2 uv = zoom * vec2(TexCoord.x * 2 - 1 - x_off, TexCoord.y * 2 - 1);
    // make c = uv for mandelbrot, z = uv for julia set
    // default exponent is 2
    float mand = mandel(uv, vec2(.65, -.7), 3, strength);
    FragColor = vec3(0.2 - mand, mand, mand);
}