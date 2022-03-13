#include "shader.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GL/glew.h>
#include <GL/glx.h>

char* load_file(const char *path) {
    FILE *f = fopen(path, "rb");
    if(!f) {
        fprintf(stderr, "Error loading file %s\n", path);
        return NULL;
    }

    const int CLUSTER = 512;

    char *buffer = malloc(CLUSTER * sizeof(char));
    int i = 0;
    int size = 0;

    while((size = fread(buffer + (CLUSTER * i), sizeof(char), CLUSTER, f)) == CLUSTER) {
        buffer = realloc(buffer, CLUSTER * (++i + 1));
    }

    buffer[i * CLUSTER + size] = 0;

    if(ferror(f)) {
        printf("Error reading file %s\n", path);

        fclose(f);
        return NULL;
    }

    fclose(f);

    return buffer;
}

shader_t* make_shader(const char *path) {
    shader_t *shader = (shader_t*)malloc(sizeof(shader_t));
    shader->vertex = -1;
    shader->fragment = -1;
    shader->program = -1;

    int path_size = strlen(path);
    char *real_path = (char*)malloc(sizeof(char) * (path_size + 12));
    strncpy(real_path, path, path_size);

    // load vertex shader
    strncpy(real_path + path_size, ".vert", 5);
    char *buffer = load_file(real_path);
    if(!buffer) {
        close_shader(shader);
        free(real_path);

        return NULL;
    }

    printf("Vertex: %s\n", buffer);

    shader->vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(shader->vertex, 1, (const char**)&buffer, NULL);
    glCompileShader(shader->vertex);

    free(buffer);

    // load fragment shader
    strncpy(real_path + path_size, ".frag", 5);
    buffer = load_file(real_path);
    if(!buffer) {
        close_shader(shader);
        free(real_path);

        return NULL;
    }

    printf("Fragment: %s\n", buffer);

    shader->fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(shader->fragment, 1, (const char**)&buffer, NULL);
    glCompileShader(shader->fragment);

    free(buffer);
    free(real_path);

    // link shader program
    shader->program = glCreateProgram();
    glAttachShader(shader->program, shader->vertex);
    glAttachShader(shader->program, shader->fragment);
    glLinkProgram(shader->program);

    return shader;
}

void close_shader(shader_t *shader) {
    if(shader == NULL) return;

    if(shader->vertex != -1) glDeleteShader(shader->vertex);
    if(shader->fragment != -1) glDeleteShader(shader->fragment);
    if(shader->program != -1) glDeleteProgram(shader->program);

    free(shader);
}