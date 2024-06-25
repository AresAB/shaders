#version 330

//uniform vec2 uResolution;
uniform sampler2D tex;
uniform sampler2D tex2;

out vec4 outColor;

float GetNoise(mat4 mat, int x, int y) {
	return float(mat[x % 4][y % 4]) * (1.0 / 16.0) - 0.5;
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
   float ditherSpread = 0.05;
   vec4 col = grayscaleColor + ditherSpread * GetNoise(bayer4, int(uv.x * resolution.x), int(uv.y * resolution.y));
   
   //posterization of colors
   col = posterize(col, 5);
   if(col.x > 0.) col = texture(tex2, vec2(col.x, 0));
   else col = texture(tex2, vec2(0.999, 0));
   
   outColor = col;
}