#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 Normal;
out vec3 View;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float time;

void main()
{
    // ampnitude of sin
    float amp = 0.8 / 100 * (sin(time)/2 + 1);
    // length between sin intervals
    float wave_len = 0.15 * (sin(time)/2 + 2);
    // speed of the wave
    float speed = 0.2;
    // this sure is a parameter
    float seed_iter = 8.;

    float freq = 2 / wave_len;

    // fractional brownian motion + sum of sins
    float sum = 0.;
    vec2 tb = vec2(0.);
    float amp_sum = 0.;
    vec3 pos = aPos;

    for(int i=0;i<8;++i){
        vec2 rand = vec2(cos(i * seed_iter), sin(i * seed_iter));
        rand /= sqrt(rand.x * rand.x + rand.y * rand.y);
        float x = dot(rand, pos.xy) * freq + time * (speed * freq);
        float wave = amp * exp(sin(x));
        vec2 dw = rand * freq * wave * cos(x);

        sum += wave;
        pos.xy += -dw * amp;
        tb += dw;
        amp_sum += amp;

        amp *= 0.83;
        freq *= 1.16;
        speed *= 1.07;
    }

    //sum *= 0.5 / 10 / amp_sum;
    //tb /= amp_sum;

    vec3 normal_vec = cross(vec3(1, 0, tb.x), vec3(0, 1, tb.y));
    normal_vec /= sqrt(normal_vec.x * normal_vec.x + normal_vec.y * normal_vec.y + normal_vec.z * normal_vec.z);

    //float wave = amp * sin((aPos.x + aPos.y) * freq + time * theta);
    float wave_final = sum;

	gl_Position = projection * view * model * vec4(aPos.x, aPos.y, wave_final, 1.0);
	TexCoord = aTexCoord;
    Normal = normal_vec;
    View = vec3(view[0][0], view[1][1], view[2][2]);
}