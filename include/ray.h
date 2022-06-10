#ifndef __RAY_H__
#define __RAY_H__

#include "math/vec.h"
#include "direction.h"

typedef struct _world_s world_t;
typedef struct {
    vec3i coord;
    direction_e face;

    int valid;
} raydata_t;

void get_block_with_ray(const world_t *w, vec3f pos, vec3f facing, raydata_t *output);

#endif
