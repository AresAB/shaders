#version 330

out vec4 outColor;

uniform vec2 uResolution;
uniform float uTime;

void main()
{
    vec2 uv = gl_FragCoord.xy/uResolution;
    
    //outColor = vec4(uv, 0., 1.);
    //outColor = vec4(0.5 * (cos(uTime+uv) + 1.), 0., 1.);
  	outColor = vec4(0.19 * (cos(uTime + uv.x + uv.y) + 1.5), 0.5 * (cos(uTime+uv) + 1.), 1.);
  	//outColor = vec4(0.5 * (cos(uTime+uv) + 1.), 0.2 * (cos(uTime + uv.x + uv.y) + 1.5), 1.);
    //outColor = vec4(0.5 + 0.5*cos(uTime+uv.yxy),1.0);
}