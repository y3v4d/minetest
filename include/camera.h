#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <stdint.h>

#include "math/vec.h"
#include "math/matrix.h"

#include "glx/shader.h"

#define CAMERA_MODE_FILL 0b01
#define CAMERA_MODE_LINE 0b10

#define CAMERA_PROJECTION_PERSPECTIVE 0b01
#define CAMERA_PROJECTION_ORTHOGRAPHIC 0b10

typedef struct {
    int left, right;
    int top, bottom;
} camera_metrics_t;

typedef struct {
    vec3f position, rotation;
    vec3f offset;
    float aspect, fov, near, far;

    camera_metrics_t metrics;

    uint8_t projection_type;
    uint8_t mode;

    vec3f facing;
    mat4_t view, projection;
} camera_t;

camera_t* camera_init(uint8_t projection, float aspect, float fov);
void camera_destroy(camera_t *c);

void camera_update(camera_t *c);
void camera_use(const camera_t *self, shader_t *shader);

#endif