#ifndef __MATRIX_H__
#define __MATRIX_H__

#define PI 3.14159265359
#define RADIANS(a) (a) * (PI / 180.f)

typedef struct _mat4_s {
    float m[16];
} mat4_t;

mat4_t mat4_identity();
mat4_t mat4_mul_mat4(mat4_t s, mat4_t o);

mat4_t mat4_perspective(float fov, float ratio, float near, float far);

mat4_t mat4_translation(float x, float y, float z);
mat4_t mat4_rotation_x(float angle);
mat4_t mat4_rotation_y(float angle);
mat4_t mat4_rotation_z(float angle);

#endif