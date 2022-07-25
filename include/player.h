#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "camera.h"
#include "utils/vec.h"
#include "utils/types.h"

#include "ray.h"

typedef struct _world_s world_t;
typedef struct {
    world_t *world;

    vec3f position;
    vec3f rotation;

    vec3f velocity;

    vec3f facing;

    camera_t *camera;
    raydata_t ray;

    uint8_t current_block;

    struct {
        float forward, right, up;
    } moving;
} player_t;

player_t* player_init(world_t *world, vec3f position, vec3f rotation);
void player_destroy(player_t *p);

void player_tick(player_t *p);

#endif
