#include "highlight.h"
#include "glx/vbo.h"
#include "math/matrix.h"

#include <stdlib.h>
#include <stdio.h>

const float HIGHLIGHT_DATA[] = {
    0.f,    1.f,    0.f,    4.f,    // 0 - left top front
    1.f,    1.f,    0.f,    4.f,    // 1 - right top front
    1.f,    0.f,    0.f,    4.f,    // 2 - right bottom front
    0.f,    0.f,    0.f,    4.f,    // 3 - left bottom front

    1.f,    1.f,    -1.f,    4.f,   // 4 - right top back
    0.f,    1.f,    -1.f,    4.f,   // 5 - left top back
    0.f,    0.f,    -1.f,    4.f,   // 6 - left bottom back
    1.f,    0.f,    -1.f,    4.f,   // 7 - right bottom back
};

const unsigned HIGHLIGHT_INDICES[] = {
    0, 1, 1, 4, 4, 5, 5, 0,
    3, 2, 2, 7, 7, 6, 6, 3,
    0, 3, 1, 2, 4, 7, 5, 6
};

highlight_t* highlight_create() {
    highlight_t *temp = (highlight_t*)malloc(sizeof(highlight_t));
    if(!temp) {
        fprintf(stderr, "Couldn't allocate memory for highlight.");
        return NULL;
    }

    temp->position = (vec3f) { 0, 0, 0 };
    temp->model = mat4_identity();

    temp->vbo = vbo_generate(GL_ARRAY_BUFFER, FALSE);
    temp->vio = vbo_generate(GL_ELEMENT_ARRAY_BUFFER, FALSE);
    temp->vao = vao_generate();

    vao_bind(&temp->vao);
    vbo_bind(&temp->vbo);
    vbo_data(&temp->vbo, sizeof(HIGHLIGHT_DATA), HIGHLIGHT_DATA);

    vbo_bind(&temp->vio);
    vbo_data(&temp->vio, sizeof(HIGHLIGHT_INDICES), HIGHLIGHT_INDICES);

    vao_attribute(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    vao_attribute(2, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(3 * sizeof(float)));

    vao_bind(NULL);

    return temp;
}

void highlight_destroy(highlight_t *p) {
    if(!p) return;

    vbo_destroy(&p->vio);
    vbo_destroy(&p->vbo);
    vao_destroy(&p->vao);

    free(p);
}

void highlight_render(highlight_t *self, shader_t *shader) {
    vao_bind(&self->vao);

    self->model = mat4_translation(self->position);

    shader_uniform(shader, "model", UNIFORM_MATRIX_4, 1, self->model.m);
    glDrawElements(GL_LINES, sizeof(HIGHLIGHT_INDICES), GL_UNSIGNED_INT, (void*)0);
}