#include "math/vec.h"
#include <math.h>

float vec2f_magnitude(vec2f v) {
    return sqrtf(v.x * v.x + v.y * v.y);
}

vec2f vec2f_normalize(vec2f v) {
    float m = vec2f_magnitude(v);

    if(m == 0) return (vec2f){ 0, 0 };
    else return (vec2f) { v.x / m, v.y / m };
}

float vec3f_magnitude(vec3f v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

vec3f vec3f_normalize(vec3f v) {
    float m = vec3f_magnitude(v);

    if(m == 0) return (vec3f){ 0, 0, 0 };
    else return (vec3f) { v.x / m, v.y / m, v.z / m };
}

vec3f vec3f_cross(vec3f self, vec3f other) {
    return (vec3f) { 
        self.y * other.z - self.z * other.y, 
        self.z * other.x - self.x * other.z, 
        self.x * other.y - self.y * other.x 
    };
}

float vec3f_dot(vec3f self, vec3f other) {
    return self.x * other.x + self.y * other.y;
}
