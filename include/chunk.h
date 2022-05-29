#ifndef __CHUNK_H__
#define __CHUNK_H__

#include <stdint.h>
#include "glx/vbo.h"
#include "glx/vao.h"
#include "glx/shader.h"

#include "mesh.h"

#include "math/vec.h"
#include "constants.h"

#define CHUNK_SIZE_X 16
#define CHUNK_SIZE_Y 16
#define CHUNK_SIZE_Z 16

typedef struct _world_s world_t;
typedef struct _chunk_s {
    world_t *world;
    uint8_t *data;

    vec2i position; // in chunks

    struct {
        mesh_t *base, *transparent;
    } meshes;
} chunk_t;

chunk_t *chunk_init(world_t *world, int x, int z);
void chunk_destroy(chunk_t *p);

void prepare_chunk(chunk_t *p);
void chunk_render(chunk_t *p, shader_t *s);

uint8_t chunk_get_block(const chunk_t *p, int x, int y, int z);
void chunk_set_block(chunk_t *p, int x, int y, int z, uint8_t type);

#endif