#ifndef __TEXT_H__
#define __TEXT_H__

#include <stdint.h>

#include "glx/vao.h"
#include "glx/vbo.h"

#include "utils/vec.h"
#include "utils/matrix.h"

#include "glx/shader.h"

#include "mesh.h"

#include "utils/font_loader.h"

#define MAX_TEXT_LENGTH 32

#define TEXT_UPDATE_POSITION 0b0001
#define TEXT_UPDATE_STRING 0b0010
#define TEXT_UPDATE_ALL 0b0011

typedef struct {
    vec3f position;

    char text[MAX_TEXT_LENGTH];
    fontbmp_t *font;

    mesh_t *mesh;
    mat4_t model;
} text_t;

text_t* text_make(fontbmp_t *font, const char *string, vec3f position);
void text_destroy(text_t *p);

void text_update(text_t *p, uint32_t flag);

void text_render(text_t *p, const shader_t *shader);

#endif
