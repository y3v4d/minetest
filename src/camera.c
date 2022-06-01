#include "camera.h"
#include "math/matrix.h"

#include <GL/glew.h>
#include <GL/glx.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

camera_t* camera_init(uint8_t projection, float aspect, float fov) {
    camera_t *temp = (camera_t*)malloc(sizeof(camera_t));
    if(!temp) {
        fprintf(stderr, "Couldn't allocate memory for camera\n");
        return NULL;
    }

    temp->position = F2VEC3F(0.f, 0.f, 0.f);
    temp->rotation = F2VEC3F(0.f, 0.f, 0.f);

    temp->aspect = aspect;
    temp->fov = fov;

    temp->near = 0.1f;
    temp->far = 10.f;

    temp->metrics = (camera_metrics_t) { 0, 0, 0, 0 };

    temp->mode = CAMERA_MODE_FILL;
    temp->projection_type = projection;

    camera_update(temp);

    return temp;
}

void camera_destroy(camera_t *c) {
    if(!c) return;

    free(c);
}

void camera_update(camera_t *c) {
    if(c->rotation.x > 90.f) c->rotation.x = 90.f;
    else if(c->rotation.x < -90.f) c->rotation.x = -90.f;
    
    c->facing = F2VEC3F(
        -sinf(RADIANS(c->rotation.y)) * cosf(RADIANS(c->rotation.x)),
        sinf(RADIANS(c->rotation.x)),
        -cosf(RADIANS(c->rotation.y)) * cosf(RADIANS(c->rotation.x))
    );

    c->view = mat4_identity();
    mat4_translate(&c->view, VEC3FINV(c->position));

    if(c->rotation.y != 0) c->view = mat4_mul_mat4(mat4_rotation_y(-c->rotation.y), c->view);
    if(c->rotation.x != 0) c->view = mat4_mul_mat4(mat4_rotation_x(-c->rotation.x), c->view);

    //mat4_translate(&c->view, F2VEC3F(0.f, 0.f, 1.f));

    if(c->projection_type == CAMERA_PROJECTION_PERSPECTIVE) 
        c->projection = mat4_perspective(c->fov, c->aspect, c->near, c->far);
    else
        c->projection = mat4_orthographic(c->metrics.left, c->metrics.right, c->metrics.top, c->metrics.bottom, c->near, c->far);
}

void camera_use(const camera_t *self, shader_t *shader) {
    glPolygonMode(GL_FRONT_AND_BACK, (self->mode == CAMERA_MODE_FILL ? GL_FILL : GL_LINE));

    shader_uniform(shader, "view", UNIFORM_MATRIX_4, 1, self->view.m);
    shader_uniform(shader, "projection", UNIFORM_MATRIX_4, 1, self->projection.m);
}