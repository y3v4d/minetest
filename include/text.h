#ifndef __TEXT_H__
#define __TEXT_H__

#include <stdint.h>

#include "glx/vao.h"
#include "glx/vbo.h"

#include "math/vec.h"
#include "math/matrix.h"

#include "glx/shader.h"

#include "utils/font_loader.h"

#define TEXT_UPDATE_POSITION 0b0001

typedef struct {
    vec3f position;

    const char *text;
    fontbmp_t *font;

    float *vertices;
    int vertices_count;

    unsigned *indices;
    int indices_count;

    int mesh_counter;

    vbo_t vbo;
    vbo_t vio;
    vao_t vao;

    mat4_t model;
} text_t;

text_t* text_make(fontbmp_t *font, const char *string, vec3f position);
void text_destroy(text_t *p);

void text_set(text_t *p, const char *string);
void text_update(text_t *p, uint32_t flag);

void text_render(text_t *p, const shader_t *shader);

#endif