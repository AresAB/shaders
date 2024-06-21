#version 330

//uniform vec2 uResolution;
uniform sampler2D tex;

out vec4 outColor;

float GetNoise(mat4 mat, int x, int y) {
	return float(mat[x % 4][y % 4]) * (1.0 / 16.0) - 0.5;
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
   
   //conversion to grayscale is done so i can implement a new color pallete
   //this is a conversion to grayscale, didn't make it myself
   float grayscale = dot(texColor.rgb, vec3(0.21, 0.72, 0.07));
   vec4 grayscaleColor = vec4(grayscale, grayscale, grayscale, 1.);
   //I didn't actually go through with the color pallete stuff, so i just undid the grayscale :)
   grayscaleColor = texColor;
   
   //this color vector is the actual final result of dithering
   float ditherSpread = 0.2;
   vec4 col = grayscaleColor + ditherSpread * GetNoise(bayer4, int(uv.x * resolution.x), int(uv.y * resolution.y));
   
   //this is the posterization/quantization of the colors
   int numberOfColors = 4;
   col = floor((numberOfColors - 1.) * col + 0.5) / (numberOfColors - 1.);
   
   outColor = col;
   
   //outColor = vec4(0., (cos(uv.x + uv.y) + 1.) * 0.5, 1., 1.);
}