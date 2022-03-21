#include "math/matrix.h"
#include "math.h"

mat4_t mat4_identity() {
    return (mat4_t) { 
        1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        0.f, 0.f, 0.f, 1.f
    };
}

mat4_t mat4_mul_mat4(mat4_t s, mat4_t o) {
    mat4_t t;
    float *tm = t.m;

    float *sm = s.m;
    float *om = o.m;

    for(int y = 0; y < 4; ++y) {
        for(int x = 0; x < 4; ++x) {
            *tm = 0;

            for(int j = 0; j < 4; ++j) {
                *tm += *(sm + j) * *(om + x + 4 * j); 
            }

            tm++;
        }

        sm += 4;
    }

    return t;
}

mat4_t mat4_perspective(float fov, float ratio, float near, float far) {
    float t = tanf(RADIANS(fov / 2.0f)) * near;
    float b = -t;

    float r = t * ratio;
    float l = -r;

    return (mat4_t) {
        (2.0f * near) / (r - l),    0.0f,                       (r + l) / (r - l),              0.0f,
        0.0f,                       (2.0f * near) / (t - b),    (t + b) / (t - b),              0.0f,
        0.0f,                       0.0f,                       -(far + near) / (far - near),   -(2.0f * far * near) / (far - near),
        0.0f,                       0.0f,                       -1.0f,                          1.0f
    };
}

mat4_t mat4_translation(float x, float y, float z) {
    return (mat4_t) {
        1.f, 0.f, 0.f, x,
        0.f, 1.f, 0.f, y,
        0.f, 0.f, 1.f, z,
        0.f, 0.f, 0.f, 1.f
    };
}

mat4_t mat4_rotation_x(float angle) {
    return (mat4_t) {
        1.f,    0.f,                    0.f,                    0.f,
        0.f,    cosf(RADIANS(angle)),   -sinf(RADIANS(angle)),  0.f,
        0.f,    sinf(RADIANS(angle)),   cosf(RADIANS(angle)),   0.f,
        0.f,    0.f,                    0.f,                    1.f
    };
}

mat4_t mat4_rotation_y(float angle) {
    return (mat4_t) {
        cosf(RADIANS(angle)),   0.f,    sinf(RADIANS(angle)),   0.f,
        0.f,                    1.f,    0.f,                    0.f,
        -sinf(RADIANS(angle)),  0.f,    cosf(RADIANS(angle)),   0.f,
        0.f,                    0.f,    0.f,                    1.f
    };
}

mat4_t mat4_rotation_z(float angle) {
    return (mat4_t) {
        cosf(RADIANS(angle)),   -sinf(RADIANS(angle)),  0.f,    0.f,
        sinf(RADIANS(angle)),   cosf(RADIANS(angle)),   0.f,    0.f,
        0.f,                    0.f,                    1.f,    0.f,
        0.f,                    0.f,                    0.f,    1.f
    };
}