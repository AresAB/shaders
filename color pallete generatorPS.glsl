#version 330

uniform vec2 uResolution;
uniform sampler2D tex;
uniform sampler2D tex2;

out vec4 outColor;

vec4 posterize(vec4 c, float level){
	return floor((level - 1.) * c + 0.5) / (level - 1.);
}

void main() {
   float numColors = 5.;
   vec2 uv = gl_FragCoord.xy/uResolution;
   
   vec4 col = vec4(0.5, 0.5, 0.5, 1.);
   
   for(int i=0;i<int(numColors);++i){
   		if (uv.x > float(i)/(numColors)) col.x = float(i)/(numColors - 1.);
   }
   
   col = vec4(col.x, col.x, col.x, 1.);
   
   //col = posterize(col, numColors);
   
   outColor = col;
}