#ifndef __VEC_H__
#define __VEC_H__

#define VEC2FINV(v) ((vec2f) { -v.x, -v.y })
#define VEC2IINV(v) ((vec2i) { -v.x, -v.y })

#define VEC3FINV(v) ((vec3f) { -v.x, -v.y, -v.z })
#define VEC3IINV(v) ((vec3i) { -v.x, -v.y, -v.z })

#define F2VEC2F(x, y) ((vec2f) { x, y })
#define I2VEC2I(x, y) ((vec2i) { x, y })

#define F2VEC3F(x, y, z) ((vec3f) { x, y, z })
#define I2VEC3I(x, y, z) ((vec3i) { x, y, z })

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

void vec2f_add(vec2f *self, vec2f other);
void vec2f_sub(vec2f *self, vec2f other);

float vec2f_magnitude(vec2f v);
vec2f vec2f_normalize(vec2f v);

void vec3f_add(vec3f *self, vec3f other);
void vec3f_sub(vec3f *self, vec3f other);

float vec3f_magnitude(vec3f v);
vec3f vec3f_normalize(vec3f v);

vec3f vec3f_cross(vec3f self, vec3f other);
float vec3f_dot(vec3f self, vec3f other);

#endif
