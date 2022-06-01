#ifndef __VEC_H__
#define __VEC_H__

#define VEC2F2I(v) ((vec2i){ (int)v.x, (int)v.y })
#define VEC2I2F(v) ((vec2f){ (float)v.x, (float)v.y })

#define VEC3F2I(v) ((vec3i){ (int)v.x, (int)v.y, (int)v.z })
#define VEC3I2F(v) ((vec3f){ (float)v.x, (float)v.y, (float)v.z })

#define VEC4F2I(v) ((vec4i){ (int)v.x, (int)v.y, (int)v.z, (int)v.w })
#define VEC4I2F(v) ((vec4f){ (float)v.x, (float)v.y, (float)v.z, (float)v.w })

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

vec3f vec3f_cross(vec3f self, vec3f other);
float vec3f_dot(vec3f self, vec3f other);

#endif
