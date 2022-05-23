#include "world.h"
#include "chunk.h"

#include <stdio.h>
#include <stdlib.h>

world_t* world_init() {
    world_t *t = (world_t*)malloc(sizeof(world_t));
    if(!t) {
        fprintf(stderr, "Couldn't allocate memory for world\n");
        return NULL;
    }

    for(int z = 0; z < CHUNK_Z; ++z) {
        for(int x = 0; x < CHUNK_X; ++x) {
            t->chunks[z * CHUNK_X + x] = initialize_chunk(t, x, -z);
            
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
    if(x >= CHUNK_X || x < 0 || -z >= CHUNK_Z || -z < 0) return NULL;

    return w->chunks[-z * CHUNK_X + x];
}

chunk_t *world_find_chunk(const world_t *w, int x, int z) {
    return find_chunk(w, x, z);
}

uint8_t world_get_block(const world_t *w, int x, int y, int z) {
    int chunk_x = x / CHUNK_SIZE_X, chunk_z = z / CHUNK_SIZE_Z;
    int in_x = x - chunk_x * CHUNK_SIZE_X, in_z = z - chunk_z * CHUNK_SIZE_Z;

    chunk_t *c = find_chunk(w, chunk_x, chunk_z);
    if(c == NULL) return 0;

    return get_chunk_block(c, in_x, y, in_z);
}

void world_set_block(const world_t *w, int x, int y, int z, uint8_t type) {
    int chunk_x = x / CHUNK_SIZE_X, chunk_z = z / CHUNK_SIZE_Z;
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

