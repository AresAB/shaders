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

float smoothstep(float x) { 
	return x*x*(3.-2.*x);
}

float rational(float x) {
	x = x * 0.5 + 0.5;
	x = x*x*x/(3.*x*x-3.*x+1.);
	return 1. - (x - 0.5) * 2.;
}

vec2 spherize(vec2 xy_off) {
    float rad = sqrt(xy_off.x * xy_off.x + xy_off.y * xy_off.y);
    if(rad > 1) discard;
    float x_max = 0;
    float y_max = 0;
    if(abs(xy_off.x) > abs(xy_off.y)) {
	x_max = 1.0 * sign(xy_off.x);
	y_max = xy_off.y / xy_off.x * x_max;
    }
    else {
	y_max = 1.0 * sign(xy_off.y);
	x_max = xy_off.x / xy_off.y * y_max;
    }
    return vec2(mix(xy_off.x, x_max, rational(rad)), mix(xy_off.y, y_max, rational(rad)));
}

void main()
{
    float zoom = 2;
    int strength = 10;
    vec2 off = vec2(0, 0);

    vec2 xy = vec2(TexCoord.x * 2 - 1, TexCoord.y * 2 - 1);
    vec2 uv = spherize(xy);
    //vec2 uv = xy;
    uv *= zoom;
    uv += off;

    float rad = sqrt(xy.x * xy.x + xy.y * xy.y);
    
    // make c = uv for mandelbrot, z = uv for julia set
    // default exponent is 2
    //float t = 0.7;
    //float mand1 = mandel(vec2(0.6, -0.3), uv, 4, strength);
    //float mand1 = mandel(uv, vec2(.65, -.7), 3, strength);
    float mand1 = mandel(uv, vec2(.65, -.7), 4, strength);
    float mand3 = mandel(vec2(0.6, -0.3), xy * 1.5 - vec2(-0.05, -0.2), 4, 10);
    //vec3 col1 = vec3(mand - mand * (1 - rad), 0.3 * rational(rad) - mand * rational(rad) + 0.1 * rational(rad*mand), mand - mand * (1 - rad));
    mand1 = mix(mand1 - 0.4, mand1 - mand3, rad);
    vec3 col1 = vec3(0.2-mand1, 0.2-mand1, mand1);
    //FragColor = mix(col1, col3, smoothstep(rad));
    FragColor = col1;
}
