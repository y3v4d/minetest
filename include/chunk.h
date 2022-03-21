#ifndef __CHUNK_H__
#define __CHUNK_H__

#include <stdint.h>

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

    uint8_t *data;
} chunk_t;

chunk_t *initialize_chunk();
void prepare_chunk(chunk_t *p);
void free_chunk(chunk_t *p);

#endif