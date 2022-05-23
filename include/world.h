#ifndef __WORLD_H__
#define __WORLD_H__

#include <unistd.h>

#include "math.h"
#include "chunk.h"

#define CHUNK_X 4
#define CHUNK_Z 4

typedef struct _world_s {
    chunk_t *chunks[CHUNK_X * CHUNK_Z];
} world_t;

world_t* world_init();
void world_destroy();

void world_render(world_t *w, shader_t *s);

uint8_t world_get_block(const world_t *w, int x, int y, int z);
void world_set_block(const world_t *w, int x, int y, int z, uint8_t type);
chunk_t *world_find_chunk(const world_t *w, int x, int z);

#endif