#include "ray.h"

#include <math.h>

void get_block_with_ray(const world_t *w, vec3f pos, vec3f facing, raydata_t *output) {
    output->coord = (vec3i) {
        floorf(pos.x),
        floorf(pos.y),
        ceilf(pos.z) // z direction is going forward on negative axis
    };
    output->valid = 0;

    float t = 0;
    vec3f check = pos;
    vec3f off = {
        facing.x >= 0 ? 1 : 0,
        facing.y >= 0 ? 1 : 0,
        facing.z >= 0 ? 0 : -1
    };

    // TODO add radius limit
    for(int i = 0; i < 16; ++i) {
        vec3f distance = {
            output->coord.x - check.x + off.x,
            output->coord.y - check.y + off.y,
            output->coord.z - check.z + off.z
        };

        distance.x /= facing.x;
        distance.y /= facing.y;
        distance.z /= facing.z;
            
        if(distance.x < distance.z) {
            if(distance.y < distance.x) {
                if(facing.y < 0) {
                    output->coord.y += -1;
                    output->face = DIRECTION_TOP;
                } else {
                    output->coord.y += 1;
                    output->face = DIRECTION_BOTTOM;
                }

                t += distance.y;
            } else {
                if(facing.x < 0) {
                    output->coord.x += -1;
                    output->face = DIRECTION_RIGHT;
                } else {
                    output->coord.x += 1;
                    output->face = DIRECTION_LEFT;
                }

                t += distance.x;
            }
        } else {
            if(distance.y < distance.z) {
                if(facing.y < 0) {
                    output->coord.y += -1;
                    output->face = DIRECTION_TOP;
                } else {
                    output->coord.y += 1;
                    output->face = DIRECTION_BOTTOM;
                }

                t += distance.y;
            } else {
                if(facing.z < 0) {
                    output->coord.z += -1;
                    output->face = DIRECTION_FRONT;
                } else {
                    output->coord.z += 1;
                    output->face = DIRECTION_BACK;
                }

                t += distance.z;
            }
        }

        check.x = pos.x + facing.x * t;
        check.y = pos.y + facing.y * t;
        check.z = pos.z + facing.z * t;

        if(world_get_block(w, output->coord.x, output->coord.y, output->coord.z)) {
            output->valid = 1;
            break;
        }
    }
}