#ifndef __SPRITE_H__
#define __SPRITE_H__

#include "math/vec.h"
#include "math/matrix.h"

#include "glx/shader.h"
#include "glx/texture.h"
#include "glx/vao.h"
#include "glx/vbo.h"

typedef struct {
    vec3f position;
    texture_t *texture;

    vbo_t vbo, vio;
    vao_t vao;

    mat4_t model;
} sprite_t;

sprite_t* sprite_init(vec3f position);
void sprite_destroy(sprite_t* p);

void sprite_render(sprite_t *self, const shader_t *shader);

#endif