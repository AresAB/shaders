#pragma once

#include <stb_image.h>

unsigned int create_texture(char *filename, GLenum image_type) {
    unsigned int tex_id;
    glGenTextures(1, &tex_id);
    glBindTexture(GL_TEXTURE_2D, tex_id); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, num_channels;
    stbi_set_flip_vertically_on_load(1);
    unsigned char *data = stbi_load(filename, &width, &height, &num_channels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, image_type, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        printf("|%s|\n(%d) ERROR: Failed to load texture \"%s\"", __FILE__, __LINE__, filename);
    }
    stbi_image_free(data);
    return tex_id;
}

unsigned int create_shader(const char* vertex_path, const char* fragment_path) {
    FILE *vert_file = fopen(vertex_path, "r");
    if(vert_file == NULL) {
        printf("|%s|\n(%d) ERROR: Unable to read vertex shader file \"%s\"", __FILE__, __LINE__, vertex_path);
        exit(1);
    }
    fseek(vert_file, 0L, SEEK_END);
    long len = ftell(vert_file) + 1;
    char *vert_code = memset(malloc(len), '\0', len);
    rewind(vert_file);
    fread(vert_code, 1, len-1, vert_file);
    fclose(vert_file);

    FILE *frag_file = fopen(fragment_path, "r");
    if(frag_file == NULL) {
        printf("|%s|\n(%d) ERROR: Unable to read fragment shader file \"%s\"", __FILE__, __LINE__, fragment_path);
        exit(1);
    }
    fseek(frag_file, 0L, SEEK_END);
    len = ftell(frag_file) + 1;
    char *frag_code = memset(malloc(len), '\0', len);
    rewind(frag_file);
    fread(frag_code, 1, len-1, frag_file);
    fclose(frag_file);

    int success;
    char info_log[1024];

    unsigned int vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert, 1, (const char**)&vert_code, NULL);
    free(vert_code);
    glCompileShader(vert);

    glGetShaderiv(vert, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vert, 1024, NULL, info_log);
        printf("|%s|\n(%d) ERROR: Vertex shader compile error\n--\n%s", __FILE__, __LINE__, info_log);
        exit(1);
    }

    unsigned int frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag, 1, (const char**)&frag_code, NULL);
    free(frag_code);
    glCompileShader(frag);

    glGetShaderiv(frag, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(frag, 1024, NULL, info_log);
        printf("|%s|\n(%d) ERROR: Fragment shader compile error\n--\n%s", __FILE__, __LINE__, info_log);
        exit(1);
    }

    unsigned int id = glCreateProgram();
    glAttachShader(id, vert);
    glAttachShader(id, frag);
    glLinkProgram(id);
    glDeleteShader(vert);
    glDeleteShader(frag);

    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(id, 1024, NULL, info_log);
        printf("|%s|\n(%d) ERROR: Program linking error\n--\n%s\n--", __FILE__, __LINE__, info_log);
        exit(1);
    }

    return id;
}

void shader_set_int(unsigned int id, char *name, int val) {
    glUniform1i(glGetUniformLocation(id, name), val); 
}

void shader_set_float(unsigned int id, char *name, float val) {
    glUniform1f(glGetUniformLocation(id, name), val); 
}