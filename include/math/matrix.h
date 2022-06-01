#ifndef __MATRIX_H__
#define __MATRIX_H__

#include "math/vec.h"

#define PI 3.14159265359
#define RADIANS(a) (a) * (PI / 180.f)

typedef struct _mat4_s {
    float m[16];
} mat4_t;

mat4_t mat4_identity();
mat4_t mat4_mul_mat4(mat4_t s, mat4_t o);

mat4_t mat4_perspective(float fov, float ratio, float near, float far);
mat4_t mat4_orthographic(float left, float right, float top, float bottom, float near, float far);

mat4_t mat4_translation(vec3f offset);
mat4_t mat4_rotation_x(float angle);
mat4_t mat4_rotation_y(float angle);
mat4_t mat4_rotation_z(float angle);

void mat4_translate(mat4_t *m, vec3f offset);

#endif