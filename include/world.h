#ifndef __WORLD_H__
#define __WORLD_H__

#include <unistd.h>

#include "chunk.h"

#include "player.h"

#define CHUNK_X 2
#define CHUNK_Z 2

typedef struct _world_s {
    chunk_t *chunks[CHUNK_X * CHUNK_Z];

    player_t *player;
} world_t;

world_t* world_init();
void world_destroy();

void world_tick(world_t *w);
void world_render(world_t *w, shader_t *s);

uint8_t world_get_block(const world_t *w, int x, int y, int z);
void world_set_block(const world_t *w, int x, int y, int z, uint8_t type);
chunk_t *world_find_chunk(const world_t *w, int x, int z);

void world_sort_chunks(world_t *w, vec3f* player_pos);

#endif