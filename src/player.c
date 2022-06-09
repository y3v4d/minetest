#include "player.h"
#include "world.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

float absf(float x) {
    return (x < 0 ? -x : x);
}

player_t* player_init(world_t *world, vec3f position, vec3f rotation) {
    player_t *temp = (player_t*)malloc(sizeof(player_t));
    if(!temp) {
        fprintf(stderr, "Couldn't allocate memory for player.");
        return NULL;
    }

    temp->position = position;
    temp->rotation = rotation;

    temp->facing = F2VEC3F(0, 0, -1);

    temp->moving.forward = 0;
    temp->moving.right = 0;
    temp->moving.up = 0;

    temp->world = world;

    return temp;
}

void player_destroy(player_t *p) {
    if(!p) return;

    free(p);
}

void player_tick(player_t *p) {
    if(p->rotation.x > 90.f) p->rotation.x = 90.f;
    else if(p->rotation.x < -90.f) p->rotation.x = -90.f;

    if(p->rotation.y > 360.f) p->rotation.y -= 360.f;
    else if(p->rotation.y < 0.f) p->rotation.y += 360.f;

    p->facing = F2VEC3F(
        -sinf(RADIANS(p->rotation.y)) * cosf(RADIANS(p->rotation.x)),
        sinf(RADIANS(p->rotation.x)),
        -cosf(RADIANS(p->rotation.y)) * cosf(RADIANS(p->rotation.x))
    );

    vec3f v = F2VEC3F(0, p->moving.up, 0);
    if(p->moving.forward != 0) {
        // Negative sin and cos where Y rotation
        // because the camera is looking at -z by default
        // (Y rotation responsible for the horizontal and depth movement)
        v.x += p->moving.forward * -sinf(RADIANS(p->rotation.y));
        //if(free_cam) world->player->velocity.y = camera->facing.y * SPEED * move;
        v.z += p->moving.forward * -cosf(RADIANS(p->rotation.y));
    }

    if(p->moving.right != 0) {
        v.x += p->moving.right * cosf(RADIANS(p->rotation.y));
        v.z += p->moving.right * -sinf(RADIANS(p->rotation.y));
    }

    {
        const float SPEED = (p->moving.forward != 0 ? absf(p->moving.forward) : absf(p->moving.right));//sqrtf(p->moving.forward * p->moving.forward + p->moving.right * p->moving.right);
        
        vec2f n = vec2f_normalize(F2VEC2F(v.x, v.z));
        v.x = n.x * SPEED;
        v.z = n.y * SPEED;
    }

    const float size_w = 0.3f;
    vec3f dir = {
        v.x >= 0 ? 1 : -1,
        0,
        v.z >= 0 ? 1 : -1
    };

    p->position.x += v.x;
    if(
        world_get_block(p->world, floorf(p->position.x + size_w * dir.x), p->position.y + 1.5f, ceilf(p->position.z + size_w)) || 
        world_get_block(p->world, floorf(p->position.x + size_w * dir.x), p->position.y + 1.5f, ceilf(p->position.z - size_w)) ||
        world_get_block(p->world, floorf(p->position.x + size_w * dir.x), p->position.y + 1.0f, ceilf(p->position.z + size_w)) || 
        world_get_block(p->world, floorf(p->position.x + size_w * dir.x), p->position.y + 1.0f, ceilf(p->position.z - size_w)) ||
        world_get_block(p->world, floorf(p->position.x + size_w * dir.x), p->position.y, ceilf(p->position.z + size_w)) || 
        world_get_block(p->world, floorf(p->position.x + size_w * dir.x), p->position.y, ceilf(p->position.z - size_w))
    ) {
        p->position.x -= v.x;
    }

    p->position.z += v.z;
    if(
        world_get_block(p->world, floorf(p->position.x + size_w), p->position.y + 1.5f, ceilf(p->position.z + size_w * dir.z)) || 
        world_get_block(p->world, floorf(p->position.x - size_w), p->position.y + 1.5f, ceilf(p->position.z + size_w * dir.z)) ||
        world_get_block(p->world, floorf(p->position.x + size_w), p->position.y + 1.0f, ceilf(p->position.z + size_w * dir.z)) || 
        world_get_block(p->world, floorf(p->position.x - size_w), p->position.y + 1.0f, ceilf(p->position.z + size_w * dir.z)) ||
        world_get_block(p->world, floorf(p->position.x + size_w), p->position.y, ceilf(p->position.z + size_w * dir.z)) || 
        world_get_block(p->world, floorf(p->position.x - size_w), p->position.y, ceilf(p->position.z + size_w * dir.z))
    ) {
        p->position.z -= v.z;
    }

    p->position.y += v.y;
    vec3i check = {
        .y = p->position.y
    };
    bool_e fall = TRUE;

    if(world_get_block(p->world, floorf(p->position.x + size_w), check.y, ceilf(p->position.z + size_w))) {
        fall = FALSE;
    } else if(world_get_block(p->world, floorf(p->position.x - size_w), check.y, ceilf(p->position.z + size_w))) {
        fall = FALSE;
    } else if(world_get_block(p->world, floorf(p->position.x + size_w), check.y, ceilf(p->position.z - size_w))) {
        fall = FALSE;
    } else if(world_get_block(p->world, floorf(p->position.x - size_w), check.y, ceilf(p->position.z - size_w))) {
        fall = FALSE;
    }

    if(!fall) {
        p->position.y = roundf(p->position.y);
        v.y = 0;
    }

    check.y = p->position.y + 1.8f;

    if(world_get_block(p->world, floorf(p->position.x + size_w), check.y, ceilf(p->position.z + size_w))) {
        p->position.y -= v.y;
        p->moving.up = 0;
    } else if(world_get_block(p->world, floorf(p->position.x - size_w), check.y, ceilf(p->position.z + size_w))) {
        p->position.y -= v.y;
        p->moving.up = 0;
    } else if(world_get_block(p->world, floorf(p->position.x + size_w), check.y, ceilf(p->position.z - size_w))) {
        p->position.y -= v.y;
        p->moving.up = 0;
    } else if(world_get_block(p->world, floorf(p->position.x - size_w), check.y, ceilf(p->position.z - size_w))) {
        p->position.y -= v.y;
        p->moving.up = 0;
    }
}