#ifndef __RAY_H__
#define __RAY_H__

#include "math/vec.h"
#include "world.h"
#include "direction.h"

typedef struct {
    vec3i coord;
    direction_e face;

    int valid;
} raydata_t;

void get_block_with_ray(const world_t *w, const vec3f *pos, const vec3f *facing, raydata_t *output);

#endif