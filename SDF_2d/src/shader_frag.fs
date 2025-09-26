#version 330 core

layout(location = 0) out vec3 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;

float sdcircle(vec2 p, float r) 
{ 
    return length(p) - r; 
} 

float sdsquare(vec2 p, vec2 b) 
{ 
    vec2 d = abs(p) - b; 
    return length(max(d, 0.)) + min(max(d.x, d.y), 0.);
} 

float sdtriangle(vec2 p, float r) 
{
    const float k = sqrt(3.);
    p.x = abs(p.x) - r;
    p.y = p.y + r/k;
    if(p.x + k * p.y > 0.) p = vec2(p.x - k * p.y, -k * p.x - p.y) / 2.; 
    p.x -= clamp(p.x, -2. * r, 0.);
    return -length(p) * sign(p.y);
} 

float smooth_min(float a, float b, float k)
{
    k *= 4;
    float h = max(k - abs(a - b), 0);
    return min(a, b) - h * h * 0.25 / k;
}

void main()
{
    float dist = sdcircle(TexCoord - vec2(0.5), 0.3); 
    float sqr = sdsquare(TexCoord - vec2(0.5), vec2(0.3, 0.2)); 
    float tri = sdtriangle(TexCoord - vec2(0.35, 0.5), 0.3); 

    // intersection
    dist = max(dist, sqr); 
    //dist = -smooth_min(-dist, -sqr, 0.04);

    // union
    dist = min(dist, tri); 
    //dist = smooth_min(dist, tri, 0.01);

    // subtraction
    dist = max(dist, -sdsquare(TexCoord - vec2(0.5), vec2(0.05, 0.05))); 
    //dist = -smooth_min(-dist, sdsquare(TexCoord - vec2(0.5), vec2(0.05, 0.05)), 0.01); 

    float dist_pos = (sign(dist) + 1.) * 0.5; 
    vec3 col = vec3(1., 0.5, 0.5) * dist_pos + vec3(0.65, 0.85, 0.85) * (1. - dist_pos);

    col *= 0.7 + 0.3 * ceil(sin(90. * dist)); 
    FragColor = col; 
}