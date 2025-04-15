#version 330 core

layout(location = 0) out vec3 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

vec2 rotate2d(vec2 uv, float _angle){
    mat2 rotation = mat2(cos(_angle),-sin(_angle),
                    sin(_angle),cos(_angle));
    
    return 0.5 + ((uv - 0.5) * rotation);
}

void main()
{
    // how many crosses (colors)
    int layer_count = 7;
    // number of times hatch repeats (tiles) in image
    float hatch_scale = 4.;

    vec3 color = texture(texture1, TexCoord).xyz;
    float gray_scale = dot(vec3(0.2989, 0.589, 0.114), color);

    int hatch_index = layer_count - int(floor(gray_scale * layer_count + 0.5));
    if (hatch_index <= 1) FragColor = vec3(1.);
    else if (hatch_index == layer_count) FragColor = vec3(0.);
    else {
        vec2 tex_size = textureSize(texture1, 0);
        vec2 uv = hatch_scale * TexCoord / tex_size;
        uv.y *= tex_size.y / tex_size.x;

        vec3 result = vec3(1.);

        for (int i = 0; i < hatch_index - 1; i++){
            float angle = fract(3.14 * i * 1.618);
            vec2 offset = rotate2d(uv, angle * 3.14 * 2);
            vec3 hatch = texture(texture2, offset * textureSize(texture2, 0)).xyz;

            result *= hatch;
        }

        FragColor = result;
    }
}
