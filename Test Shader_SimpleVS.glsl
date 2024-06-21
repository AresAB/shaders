#version 330

uniform mat4 matVP;
uniform mat4 matGeo;

layout (location = 0) in vec2 pos;

void main() {
   //gl_Position = matVP * matGeo * vec4(pos, 0.0f, 1);
   gl_Position = vec4(pos, 0.0f, 1);
}
