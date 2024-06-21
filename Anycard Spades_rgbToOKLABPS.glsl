#version 330

//uniform vec2 uResolution;
uniform sampler2D tex;

out vec4 outColor;

vec3 linear_srgb_to_oklab(vec4 c) {
    float l = 0.4122214708 * c.r + 0.5363325363 * c.g + 0.0514459929 * c.b;
	float m = 0.2119034982 * c.r + 0.6806995451 * c.g + 0.1073969566 * c.b;
	float s = 0.0883024619 * c.r + 0.2817188376 * c.g + 0.6299787005 * c.b;

    float l_ = pow(l, 1./3.);
    float m_ = pow(m, 1./3.);
    float s_ = pow(s, 1./3.);

    return vec3(0.2104542553 * l_ + 0.7936177850 * m_ - 0.0040720468 * s_, 1.9779984951 * l_ - 2.4285922050 * m_ + 0.4505937099 * s_, 0.0259040371 * l_ + 0.7827717662 * m_ - 0.8086757660 * s_);
}

vec3 oklab_to_linear_srgb(vec3 c) {
    float l_ = c.r + 0.3963377774 * c.g + 0.2158037573 * c.b;
    float m_ = c.r - 0.1055613458 * c.g - 0.0638541728 * c.b;
    float s_ = c.r - 0.0894841775 * c.g - 1.2914855480 * c.b;

    float l = l_*l_*l_;
    float m = m_*m_*m_;
    float s = s_*s_*s_;

    return vec3(4.0767416621 * l - 3.3077115913 * m + 0.2309699292 * s, -1.2684380046 * l + 2.6097574011 * m - 0.3413193965 * s, -0.0041960863 * l - 0.7034186147 * m + 1.7076147010 * s);
}

float posterize(float c, float levels){
  return ceil(c * levels - 0.5)/levels;
}

void main() {
   vec2 resolution = vec2(360., 504.);
   //vec2 uv = gl_FragCoord.xy/resolution;
   vec2 uv = vec2(min(gl_FragCoord.x/resolution.x, 1.), min(gl_FragCoord.y/resolution.y, 1.));
   uv.x = max(floor(uv.y), uv.x);
   uv.y = max(floor(uv.x), uv.y);
   
   vec4 texColor = texture(tex, uv.xy);
   
   vec3 col = linear_srgb_to_oklab(texColor);
  
   col.x = posterize(col.x, 3.); //c : 3, d : 13, s : none, h : none
   col.y = posterize(col.y, 3.); //c : 3, d : 15, h : 8
   col.b = posterize(col.b, 10.); //c : 10, d : 10, h : 10
  
   col = oklab_to_linear_srgb(col);
   
   outColor = vec4(col, 1.);
   
   //outColor = vec4(0., (cos(uv.x + uv.y) + 1.) * 0.5, 1., 1.);
}