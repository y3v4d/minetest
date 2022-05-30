#include "sprite.h"
#include "glx/vao.h"
#include "glx/vbo.h"

#include <stdio.h>
#include <stdlib.h>

const float DATA[] = {
    -1.f,  1.f, 0.f, 0.f, 1.f,
     1.f,  1.f, 0.f, 1.f, 1.f,
     1.f, -1.f, 0.f, 1.f, 0.f,
    -1.f, -1.f, 0.f, 0.f, 0.f
};

const unsigned INDICES[] = {
    2, 1, 0,
    0, 3, 2
};

sprite_t* sprite_init(vec3f position) {
    sprite_t *temp = (sprite_t*)malloc(sizeof(sprite_t));
    if(!temp) {
        fprintf(stderr, "Couldn't allocate memory for sprite.");
        return NULL;
    }

    temp->position = position;
    temp->model = mat4_translation(position.x, position.y, position.z);

    temp->vbo = vbo_generate(GL_ARRAY_BUFFER, FALSE);
    temp->vio = vbo_generate(GL_ELEMENT_ARRAY_BUFFER, FALSE);
    temp->vao = vao_generate();

    vao_bind(&temp->vao);
    vbo_bind(&temp->vbo);
    vbo_data(&temp->vbo, sizeof(DATA), DATA);

    vbo_bind(&temp->vio);
    vbo_data(&temp->vio, sizeof(INDICES), INDICES);

    vao_attribute(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    vao_attribute(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    vao_bind(NULL);

    return temp;
}

void sprite_destroy(sprite_t* p) {
    if(!p) return;

    vbo_destroy(&p->vbo);
    vbo_destroy(&p->vio);
    vao_destroy(&p->vao);

    free(p);
}

void sprite_render(sprite_t *self, const shader_t *shader) {
    vao_bind(&self->vao);
    vbo_bind(&self->vbo);
    vbo_bind(&self->vio);

    glActiveTexture(GL_TEXTURE0);
    texture_bind(self->texture);

    shader_uniform(shader, "model", UNIFORM_MATRIX_4, 1, self->model.m);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
}