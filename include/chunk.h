#ifndef __CHUNK_H__
#define __CHUNK_H__

#include <stdint.h>
#include "glx/vbo.h"
#include "glx/vao.h"

#define CHUNK_SIZE_X 16
#define CHUNK_SIZE_Y 2
#define CHUNK_SIZE_Z 16

typedef enum {
    FRONT   = 0,
    BACK    = 1,
    RIGHT   = 2,
    LEFT    = 3,
    TOP     = 4,
    BOTTOM  = 5
} direction_e;

typedef struct _chunk_s {
    // ONE VERTEX DATA
    // x y z u v layer
    float *vertices;
    int vertex_count;

    int *indices;
    int index_count;

    int mesh_counter;

    vbo_t vbo, vio;
    vao_t vao;

    uint8_t *data;
} chunk_t;

chunk_t *initialize_chunk();
void prepare_chunk(chunk_t *p);
void chunk_render(chunk_t *p);
void free_chunk(chunk_t *p);

uint8_t get_chunk_block(chunk_t *p, int x, int y, int z);
void set_chunk_block(chunk_t *p, int x, int y, int z, uint8_t type);

#endif