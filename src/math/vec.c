#include "math/vec.h"
#include <math.h>

float magnitude(vec3f v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

vec3f normalize(vec3f v) {
    float m = magnitude(v);

    if(m == 0) return (vec3f){ 0, 0, 0 };
    else return (vec3f) { v.x / m, v.y / m, v.z / m };
}

vec3f cross(vec3f v0, vec3f v1) {
    return (vec3f){ v0.y * v1.z - v0.z * v1.y, v0.z * v1.x - v0.x * v1.z, v0.x * v1.y - v0.y * v1.x };
}

float dot(vec3f v0, vec3f v1) {
    return v0.x * v1.x + v0.y * v1.y;
}
