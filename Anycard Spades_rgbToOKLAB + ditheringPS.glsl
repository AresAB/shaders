#version 330

//uniform vec2 uResolution;
uniform sampler2D tex;

out vec4 outColor;

float GetNoise(mat4 mat, int x, int y) {
	return float(mat[x % 4][y % 4]) * (1.0 / 16.0) - 0.5;
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

float posterize(float c, float level){
	return floor((level - 1.) * c + 0.5) / (level - 1.);
}

vec4 posterize(vec4 c, float level){
	return floor((level - 1.) * c + 0.5) / (level - 1.);
}

void main() {
   vec2 resolution = vec2(360., 504.);
   //vec2 uv = gl_FragCoord.xy/resolution;
   vec2 uv = vec2(min(gl_FragCoord.x/resolution.x, 1.), min(gl_FragCoord.y/resolution.y, 1.));
   uv.x = max(floor(uv.y), uv.x);
   uv.y = max(floor(uv.x), uv.y);
   
   mat4 bayer4;
   bayer4[0] = vec4(0, 12, 3, 15);
   bayer4[1] = vec4(8, 4, 11, 7);
   bayer4[2] = vec4(2, 14, 1, 13);
   bayer4[3] = vec4(10, 6, 9, 5);
   
   vec4 texColor = texture(tex, uv.xy);
   
   float grayscale = dot(texColor.rgb, vec3(0.21, 0.72, 0.07));
   vec4 grayscaleColor = vec4(grayscale, grayscale, grayscale, 1.);
   
   //this color vector is the actual final result of dithering
   float ditherSpread = 0.2;
   vec4 col = grayscaleColor + ditherSpread * GetNoise(bayer4, int(uv.x * resolution.x), int(uv.y * resolution.y));
   
   //posterization of colors
   col = posterize(col, 4);
   
   //this is the posterization/quantization of the colors
   vec4 oklab_col = linear_srgb_to_oklch(col);
   float newLuminance = 0.;
   float newLuminanceRange = 1.;
   float newChroma = 0.2;
   float newChromaRange = 0.7;
   float newHue = 2.3;
   float newHueRange = 2.;
   oklab_col.x = newLuminance + (oklab_col.x * newLuminanceRange);
   oklab_col.y = newChroma;// + (oklab_col.y * newChromaRange);
   oklab_col.z = newHue + (oklab_col.z * newHueRange);
   
   col = oklch_to_linear_srgb(oklab_col);
   
   outColor = col;
}