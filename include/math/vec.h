#ifndef __VEC_H__
#define __VEC_H__

typedef struct {
    int x, y;
} vec2i;

typedef struct {
    float x, y;
} vec2f;

typedef struct {
    int x, y, z;
} vec3i;

typedef struct {
    float x, y, z;
} vec3f;

typedef struct {
    int x, y, z, w;
} vec4i;

typedef struct {
    float x, y, z, w;
} vec4f;

float vec2f_magnitude(vec2f v);
vec2f vec2f_normalize(vec2f v);

float vec3f_magnitude(vec3f v);
vec3f vec3f_normalize(vec3f v);

vec3f vec3f_cross(vec3f v0, vec3f v1);
float vec3f_dot(vec3f v0, vec3f v1);

#endif
