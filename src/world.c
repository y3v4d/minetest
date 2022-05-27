#include "world.h"
#include "chunk.h"

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
        float highest_distance = distance_between(pos.x, pos.z, w->chunks[highest]->x * CHUNK_SIZE_X, w->chunks[highest]->z * CHUNK_SIZE_Z);

        for(int j = i + 1; j < TOTAL; ++j) {
            float current_distance = distance_between(pos.x, pos.z, w->chunks[j]->x * CHUNK_SIZE_X, w->chunks[j]->z * CHUNK_SIZE_Z);
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
            t->chunks[z * CHUNK_X + x] = initialize_chunk(t, x - 1, -z + 1);
        }
    }

    for(int z = 0; z < CHUNK_Z; ++z) {
        for(int x = 0; x < CHUNK_X; ++x) {
            prepare_chunk(t->chunks[z * CHUNK_X + x]);
        }
    }

    return t;
}

void world_destroy(world_t *p) {
    if(!p) return;

    for(int i = 0; i < CHUNK_X * CHUNK_Z; ++i) {
        if(p->chunks[i]) free_chunk(p->chunks[i]);
    }

    free(p);
}

chunk_t* find_chunk(const world_t *w, int x, int z) {
    //if(x >= CHUNK_X || x < 0 || -z >= CHUNK_Z || -z < 0) return NULL;

    for(int i = 0; i < CHUNK_X * CHUNK_Z; ++i) {
        if(w->chunks[i]->x == x && w->chunks[i]->z == z) return w->chunks[i];
    }

    return NULL;
}

chunk_t *world_find_chunk(const world_t *w, int x, int z) {
    return find_chunk(w, x, z);
}

uint8_t world_get_block(const world_t *w, int x, int y, int z) {
    int chunk_x = (int)floorf((float)x / CHUNK_SIZE_X), chunk_z = (int)ceilf((float)z / CHUNK_SIZE_Z);
    int in_x = x - chunk_x * CHUNK_SIZE_X, in_z = z - chunk_z * CHUNK_SIZE_Z;

    chunk_t *c = find_chunk(w, chunk_x, chunk_z);
    if(c == NULL) return 0;

    return get_chunk_block(c, in_x, y, in_z);
}

void world_set_block(const world_t *w, int x, int y, int z, uint8_t type) {
    int chunk_x = (int)floorf((float)x / CHUNK_SIZE_X), chunk_z = (int)ceilf((float)z / CHUNK_SIZE_Z);
    int in_x = x - chunk_x * CHUNK_SIZE_X, in_z = z - chunk_z * CHUNK_SIZE_Z;

    chunk_t *c = find_chunk(w, chunk_x, chunk_z);
    if(!c) return;

    set_chunk_block(c, in_x, y, in_z, type);
    prepare_chunk(c);
}

void world_render(world_t *w, shader_t *s) {
    for(int z = 0; z < CHUNK_Z; ++z) {
        for(int x = 0; x < CHUNK_X; ++x) {
            chunk_render(w->chunks[z * CHUNK_X + x], s);
        }
    }
}

