#include "mesh.h"
#include "chunk.h"

#include <stdio.h>
#include <stdlib.h>

mesh_t* mesh_init(size_t vertex_capacity, size_t index_capacity) {
    mesh_t *temp = (mesh_t*)malloc(sizeof(mesh_t));
    if(!temp) {
        fprintf(stderr, "Couldn't allocate memory for the mesh\n");
        return NULL;
    }

    temp->vertices = mesh_buffer_init(vertex_capacity, sizeof(float));
    if(!temp->vertices) {
        mesh_destroy(temp);
        return NULL;
    }

    temp->indices = mesh_buffer_init(index_capacity, sizeof(unsigned));
    if(!temp->indices) {
        mesh_destroy(temp);
        return NULL;
    }

    temp->vertex_counter = 0;

    temp->vbo = vbo_generate(GL_ARRAY_BUFFER, TRUE);
    temp->vio = vbo_generate(GL_ELEMENT_ARRAY_BUFFER, TRUE);
    temp->vao = vao_generate();

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