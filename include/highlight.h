#ifndef __HIGHLIGHT_H__
#define __HIGHLIGHT_H__

#include "utils/vec.h"
#include "utils/matrix.h"

#include "glx/vbo.h"
#include "glx/vao.h"
#include "glx/shader.h"

typedef struct {
    vec3f position;

    vbo_t vbo, vio;
    vao_t vao;

    mat4_t model;
} highlight_t;

highlight_t* highlight_create();
void highlight_destroy(highlight_t *p);

void highlight_render(highlight_t *self, shader_t *shader);

#endif
