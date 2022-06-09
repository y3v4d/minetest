#include "world.h"
#include "camera.h"
#include "chunk.h"
#include "math/vec.h"
#include "player.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

float distance_between(float x0, float z0, float x1, float z1) {
    return sqrtf((x1 - x0) * (x1 - x0) + (z1 - z0) * (z1 - z0));
}

void world_sort_chunks(world_t *w, vec3f* player_pos) {
    vec3f pos = {
        (int)(player_pos->x / 16) * 16,
        (int)(player_pos->y / 16) * 16,
        (int)(player_pos->z / 16) * 16
    };

    const int TOTAL = CHUNK_X * CHUNK_Z;

    for(int i = 0; i < TOTAL; ++i) {
        int highest = i;
        float highest_distance = distance_between(pos.x, pos.z, w->chunks[highest]->position.x * CHUNK_SIZE_X, w->chunks[highest]->position.y * CHUNK_SIZE_Z);

        for(int j = i + 1; j < TOTAL; ++j) {
            float current_distance = distance_between(pos.x, pos.z, w->chunks[j]->position.x * CHUNK_SIZE_X, w->chunks[j]->position.y * CHUNK_SIZE_Z);
            if(current_distance > highest_distance) {
                current_distance = highest_distance;
                highest = j;
            }
        }

        if(highest != i) {
            chunk_t *temp = w->chunks[highest];
            w->chunks[highest] = w->chunks[i];
            w->chunks[i] = temp;
        }
    }
}

world_t* world_init() {
    world_t *t = (world_t*)malloc(sizeof(world_t));
    if(!t) {
        fprintf(stderr, "Couldn't allocate memory for world\n");
        return NULL;
    }

    for(int z = 0; z < CHUNK_Z; ++z) {
        for(int x = 0; x < CHUNK_X; ++x) {
            t->chunks[z * CHUNK_X + x] = chunk_init(t, x - 1, -z + 1);
        }
    }

    for(int z = 0; z < CHUNK_Z; ++z) {
        for(int x = 0; x < CHUNK_X; ++x) {
            prepare_chunk(t->chunks[z * CHUNK_X + x]);
        }
    }

    t->player = player_init(t, F2VEC3F(0, 6.0f, 0), F2VEC3F(0, 0, 0));
    if(!t->player) {
        world_destroy(t);
        return NULL;
    }

    return t;
}

void world_destroy(world_t *p) {
    if(!p) return;

    if(p->player) player_destroy(p->player);
    for(int i = 0; i < CHUNK_X * CHUNK_Z; ++i) {
        if(p->chunks[i]) chunk_destroy(p->chunks[i]);
    }

    free(p);
}

chunk_t *world_find_chunk(const world_t *w, int x, int z) {
    for(int i = 0; i < CHUNK_X * CHUNK_Z; ++i) {
        if(w->chunks[i]->position.x == x && w->chunks[i]->position.y == z) return w->chunks[i];
    }

    return NULL;
}

uint8_t world_get_block(const world_t *w, int x, int y, int z) {
    int chunk_x = (int)floorf((float)x / CHUNK_SIZE_X), chunk_z = (int)ceilf((float)z / CHUNK_SIZE_Z);
    int in_x = x - chunk_x * CHUNK_SIZE_X, in_z = z - chunk_z * CHUNK_SIZE_Z;

    chunk_t *c = world_find_chunk(w, chunk_x, chunk_z);
    if(c == NULL) return 0;

    return chunk_get_block(c, in_x, y, in_z);
}

void world_set_block(const world_t *w, int x, int y, int z, uint8_t type) {
    int chunk_x = (int)floorf((float)x / CHUNK_SIZE_X), chunk_z = (int)ceilf((float)z / CHUNK_SIZE_Z);
    int in_x = x - chunk_x * CHUNK_SIZE_X, in_z = z - chunk_z * CHUNK_SIZE_Z;

    chunk_t *c = world_find_chunk(w, chunk_x, chunk_z);
    if(!c) return;

    chunk_set_block(c, in_x, y, in_z, type);
    prepare_chunk(c);
}

void world_tick(world_t *w) {
    player_tick(w->player);
}

void world_render(world_t *w, shader_t *s) {
    for(int z = 0; z < CHUNK_Z; ++z) {
        for(int x = 0; x < CHUNK_X; ++x) {
            chunk_render(w->chunks[z * CHUNK_X + x], s);
        }
    }
}

