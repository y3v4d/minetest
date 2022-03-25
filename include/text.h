#ifndef __TEXT_H__
#define __TEXT_H__

#include "glx/vao.h"
#include "glx/vbo.h"

#include "utils/font_loader.h"

typedef struct {
    const char *text;

    float *vertices;
    int vertices_count;

    unsigned *indices;
    int indices_count;

    int mesh_counter;

    vbo_t vbo;
    vbo_t vio;
    vao_t vao;

    fontbmp_t *font;
} text_t;

text_t* text_make(fontbmp_t *font);
void text_destroy(text_t *p);

void text_set(text_t *p, const char *string);

void text_render(text_t *p);

#endif