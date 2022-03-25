#ifndef __RAY_H__
#define __RAY_H__

#include <math/vec.h>
#include <chunk.h>

typedef struct {
    vec3i coord;
    direction_e face;

    int valid;
} raydata_t;

void get_block_with_ray(const chunk_t *chunk, const vec3f *pos, const vec3f *facing, raydata_t *output);

#endif