#ifndef __DIRECTION_H__
#define __DIRECTION_H__

#include "utils/vec.h"

typedef enum {
    DIRECTION_FRONT   = 0,
    DIRECTION_BACK    = 1,
    DIRECTION_RIGHT   = 2,
    DIRECTION_LEFT    = 3,
    DIRECTION_TOP     = 4,
    DIRECTION_BOTTOM  = 5
} direction_e;

extern const vec3i _dir_to_vec3i[6];
extern const char* _dir_to_str[6];

#define DIR2VEC3I(d) (_dir_to_vec3i[d])
#define DIR2STR(d) (_dir_to_str[d])

#endif
