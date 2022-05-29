#ifndef __MESH_H__
#define __MESH_H__

#include "glx/vbo.h"
#include "glx/vao.h"

// ONE VERTEX DATA
// x y z u v layer

typedef struct _mesh_buffer_s {
    void *data;

    size_t index, capacity;
} mesh_buffer_t;

typedef struct _mesh_s {
    mesh_buffer_t *vertices, *indices;

    int vertex_counter;

    vbo_t vbo, vio;
    vao_t vao;
} mesh_t;

mesh_t* mesh_init();
void mesh_destroy(mesh_t *m);

void mesh_prepare(mesh_t *m);
void mesh_finalize(mesh_t *m);

mesh_buffer_t* mesh_buffer_init(size_t size, size_t bytes_per_data);
void mesh_buffer_destroy(mesh_buffer_t *m);

#endif