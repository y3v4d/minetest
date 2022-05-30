#include "camera.h"
#include "math/matrix.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

camera_t* camera_init(float aspect, float fov) {
    camera_t *temp = (camera_t*)malloc(sizeof(camera_t));
    if(!temp) {
        fprintf(stderr, "Couldn't allocate memory for camera\n");
        return NULL;
    }

    temp->position = (vec3f) { 0.f, 7.5f, 0.f };
    temp->rotation = (vec3f) { 0.f, 0.f, 0.f };

    temp->aspect = aspect;
    temp->fov = fov;

    temp->near = 0.1f;
    temp->far = 10.f;

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
    
    c->facing = (vec3f) {
        -sinf(RADIANS(c->rotation.y)) * cosf(RADIANS(c->rotation.x)),
        sinf(RADIANS(c->rotation.x)),
        -cosf(RADIANS(c->rotation.y)) * cosf(RADIANS(c->rotation.x))
    };

    c->view = mat4_identity();
    mat4_translate(&c->view, -c->position.x, -c->position.y, -c->position.z);

    c->view = mat4_mul_mat4(mat4_rotation_y(-c->rotation.y), c->view);
    c->view = mat4_mul_mat4(mat4_rotation_x(-c->rotation.x), c->view);

    mat4_translate(&c->view, 0.f, 0.f, 0.6f);

    c->projection = mat4_perspective(c->fov, c->aspect, c->near, c->far); 
}