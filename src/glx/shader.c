#include "glx/shader.h"

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

GLint check_compilation(GLuint shader) {
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if(success == GL_FALSE) {
        GLint length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

        GLchar *log = (GLchar*)malloc(sizeof(GLchar) * length);
        glGetShaderInfoLog(shader, length, &length, log);

        printf("Error compiling shader:\n%s\n", log);

        free(log);
    }

    return success;
}

shader_t* shader_init(const char *path) {
    shader_t *shader = (shader_t*)malloc(sizeof(shader_t));

    int path_size = strlen(path);
    char *real_path = (char*)malloc(sizeof(char) * (path_size + 12));
    strncpy(real_path, path, path_size);

    // load vertex shader
    strncpy(real_path + path_size, ".vert", 5);
    real_path[path_size + 5] = 0;
    char *buffer = load_file(real_path);
    if(!buffer) {
        shader_destroy(shader);
        free(real_path);

        return NULL;
    }

    shader->vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(shader->vertex, 1, (const char**)&buffer, NULL);
    glCompileShader(shader->vertex);

    free(buffer);

    if(!check_compilation(shader->vertex)) {
        free(real_path);
        shader_destroy(shader);

        return NULL;
    }

    // load fragment shader
    strncpy(real_path + path_size, ".frag", 5);
    buffer = load_file(real_path);
    if(!buffer) {
        shader_destroy(shader);
        free(real_path);

        return NULL;
    }

    shader->fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(shader->fragment, 1, (const char**)&buffer, NULL);
    glCompileShader(shader->fragment);

    free(buffer);
    free(real_path);

    if(!check_compilation(shader->fragment)) {
        shader_destroy(shader);

        return NULL;
    }

    // link shader program
    shader->program = glCreateProgram();
    glAttachShader(shader->program, shader->vertex);
    glAttachShader(shader->program, shader->fragment);
    glLinkProgram(shader->program);

    return shader;
}

void shader_use(const shader_t *shader) {
    glUseProgram(shader->program);
}

void shader_uniform(const shader_t *shader, const char *name, uniform_type_e t, size_t count, const void *data) {
    if(t == UNIFORM_MATRIX_4) {
        glUniformMatrix4fv(glGetUniformLocation(shader->program, name), count, GL_TRUE, (const GLfloat*)data);
    }
}

void shader_destroy(shader_t *shader) {
    if(shader == NULL) return;

    glDeleteShader(shader->vertex);
    glDeleteShader(shader->fragment);
    glDeleteProgram(shader->program);

    free(shader);
}
