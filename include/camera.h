#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "math/vec.h"
#include "math/matrix.h"

typedef struct {
    vec3f position, rotation;
    vec3f facing;

    mat4_t view, projection;
} camera_t;

camera_t* camera_init();
void camera_destroy(camera_t *c);

void camera_update(camera_t *c);

#endif