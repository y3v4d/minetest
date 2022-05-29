#include "mesh.h"
#include "chunk.h"

#include <stdio.h>
#include <stdlib.h>

mesh_t* mesh_init() {
    const size_t SIZE = CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z;
    
    mesh_t *temp = (mesh_t*)malloc(sizeof(mesh_t));
    if(!temp) {
        fprintf(stderr, "Couldn't allocate memory for the mesh\n");
        return NULL;
    }

    temp->vertices = mesh_buffer_init(SIZE * 6 * 6 * 4, sizeof(float));
    if(!temp->vertices) {
        mesh_destroy(temp);
        return NULL;
    }

    temp->indices = mesh_buffer_init(SIZE * 6 * 6, sizeof(unsigned));
    if(!temp->indices) {
        mesh_destroy(temp);
        return NULL;
    }

    temp->vertex_counter = 0;

    temp->vbo = vbo_generate(GL_ARRAY_BUFFER, TRUE);
    temp->vio = vbo_generate(GL_ELEMENT_ARRAY_BUFFER, TRUE);
    temp->vao = vao_generate();

    vao_bind(&temp->vao);
    vbo_bind(&temp->vbo);
    vbo_bind(&temp->vio);
    vao_attribute(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    vao_attribute(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    vao_attribute(2, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(5 * sizeof(float)));

    vao_bind(NULL);

    return temp;
}

void mesh_destroy(mesh_t *m) {
    if(!m) return;

    mesh_buffer_destroy(m->vertices);
    mesh_buffer_destroy(m->indices);

    vbo_destroy(&m->vbo);
    vbo_destroy(&m->vio);
    vao_destroy(&m->vao);

    free(m);
}

void mesh_prepare(mesh_t *m) {
    m->vertex_counter = 0;
    m->vertices->index = 0;
    m->indices->index = 0;
}

void mesh_finalize(mesh_t *m) {
    vbo_bind(&m->vbo);
    vbo_data(&m->vbo, m->vertex_counter * sizeof(float), m->vertices->data);
    vbo_bind(&m->vio);
    vbo_data(&m->vio, m->indices->index * sizeof(GLuint), m->indices->data);
}

mesh_buffer_t* mesh_buffer_init(size_t size, size_t bytes_per_data) {
    mesh_buffer_t *temp = (mesh_buffer_t*)malloc(sizeof(mesh_buffer_t));
    if(!temp) {
        fprintf(stderr, "Couldn't allocate memory for mesh buffer\n");
        return NULL;
    }

    temp->data = malloc(size * bytes_per_data);
    if(!temp->data) {
        fprintf(stderr, "Couldn't allocate memory for mesh buffer data\n");
        free(temp);

        return NULL;
    }

    temp->index = 0;
    temp->capacity = size;

    return temp;
}

void mesh_buffer_destroy(mesh_buffer_t *m) {
    if(!m) return;

    if(m->data) free(m->data);
    free(m);
}