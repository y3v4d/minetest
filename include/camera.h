#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <stdint.h>

#include "math/vec.h"
#include "math/matrix.h"

#define CAMERA_MODE_FILL 0b01
#define CAMERA_MODE_LINE 0b10

typedef struct {
    vec3f position, rotation;
    float aspect, fov, near, far;

    uint8_t mode;

    vec3f facing;
    mat4_t view, projection;
} camera_t;

camera_t* camera_init(float aspect, float fov);
void camera_destroy(camera_t *c);

void camera_update(camera_t *c);
void camera_use(const camera_t *c);

#endif