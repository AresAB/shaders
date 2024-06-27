#version 330

uniform vec2 uResolution;
uniform sampler2D tex;
uniform sampler2D tex2;

out vec4 outColor;

vec4 posterize(vec4 c, float level){
	return floor((level - 1.) * c + 0.5) / (level - 1.);
}

float atan2(float x, float y) {
    float angle = asin(x) > 0 ? acos(y) : -acos(y);
    return angle;
}

vec4 linear_srgb_to_oklch(vec4 c) {
	//oklab conversions
    float l = 0.4122214708 * c.r + 0.5363325363 * c.g + 0.0514459929 * c.b;
	float m = 0.2119034982 * c.r + 0.6806995451 * c.g + 0.1073969566 * c.b;
	float s = 0.0883024619 * c.r + 0.2817188376 * c.g + 0.6299787005 * c.b;

    float l_ = pow(l, 1./3.);
    float m_ = pow(m, 1./3.);
    float s_ = pow(s, 1./3.);

	vec4 oklab = vec4(0.2104542553 * l_ + 0.7936177850 * m_ - 0.0040720468 * s_, 1.9779984951 * l_ - 2.4285922050 * m_ + 0.4505937099 * s_, 0.0259040371 * l_ + 0.7827717662 * m_ - 0.8086757660 * s_, 1.);
	
	//oklab to oklch conversions
	vec4 oklch = vec4(oklab.x, pow(pow(oklab.y, 2) + pow(oklab.z, 2), 0.5), atan2(oklab.z, oklab.y), oklab.w);
	
    return oklch;
}

vec4 oklch_to_linear_srgb(vec4 col) {
	//oklch to oklab conversions
	vec3 c = vec3(col.x, col.y * cos(col.z), col.y * sin(col.z));

	//oklab to srgb conversions
    float l_ = c.r + 0.3963377774 * c.g + 0.2158037573 * c.b;
    float m_ = c.r - 0.1055613458 * c.g - 0.0638541728 * c.b;
    float s_ = c.r - 0.0894841775 * c.g - 1.2914855480 * c.b;

    float l = l_*l_*l_;
    float m = m_*m_*m_;
    float s = s_*s_*s_;

    return vec4(4.0767416621 * l - 3.3077115913 * m + 0.2309699292 * s, -1.2684380046 * l + 2.6097574011 * m - 0.3413193965 * s, -0.0041960863 * l - 0.7034186147 * m + 1.7076147010 * s, 1.);
}

void main() {
   float num_colors = 5.;
   vec2 uv = gl_FragCoord.xy/uResolution;
   
   vec4 col = vec4(uv.x, uv.x, uv.x, 1.);
   
   col = posterize(col, num_colors);
   
   vec4 oklch_col = linear_srgb_to_oklch(col);
   
   float new_luminance = 0.2;
   float new_chroma = 0.5;
   float new_hue = 0.5;
   float luminance_spread = 0.2;
   float hue_spread = 0.05;
   
   //oklch_col.x = new_luminance + (luminance_spread * col.x * (num_colors - 1.));
   //oklch_col.y = new_chroma;
   oklch_col.z = new_hue + (hue_spread * col.x * (num_colors - 1.));
   
   col = oklch_to_linear_srgb(oklch_col);
   
   outColor = col;
}