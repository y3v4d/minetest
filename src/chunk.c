#include "chunk.h"
#include "glx/vao.h"
#include "glx/vbo.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

const int CUBE_INDICES[] = {
    0, 1, 2, 3, 0, 2,   // front
    4, 5, 6, 7, 4, 6,   // back
    1, 4, 7, 2, 1, 7,   // right
    5, 0, 3, 6, 5, 3,   // left
    5, 4, 1, 0, 5, 1,   // top
    3, 2, 7, 6, 3, 7    // bottom
};

const float CUBE_VERTICES[] = {
    0.f,    1.f,    0.f,    // 0 - left top front
    1.f,    1.f,    0.f,    // 1 - right top front
    1.f,    0.f,    0.f,    // 2 - right bottom front
    0.f,    0.f,    0.f,    // 3 - left bottom front

    1.f,    1.f,    -1.f,   // 4 - right top back
    0.f,    1.f,    -1.f,   // 5 - left top back
    0.f,    0.f,    -1.f,   // 6 - left bottom back
    1.f,    0.f,    -1.f,   // 7 - right bottom back
};

const float TEX_UV[] = {
    0.f,    1.f,    // 0 - left top
    1.f,    1.f,    // 1 - right top
    1.f,    0.f,    // 2 - right bottom
    0.f,    0.f     // 3 - left bottom
};

chunk_t *initialize_chunk() {
    chunk_t *p = (chunk_t*)malloc(sizeof(chunk_t));
    memset(p, 0, sizeof(chunk_t));

    const size_t SIZE = CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z;
    p->data = (uint8_t*)malloc(sizeof(uint8_t) * SIZE);

    p->vertices = (float*)malloc(sizeof(float) * SIZE * 6 * 4 * 6);
    p->indices = (int*)malloc(sizeof(float) * SIZE * 6 * 6);

    memset(p->data, 1, sizeof(uint8_t) * SIZE);

    p->vbo = vbo_generate(GL_ARRAY_BUFFER, TRUE);
    p->vio = vbo_generate(GL_ELEMENT_ARRAY_BUFFER, TRUE);
    p->vao = vao_generate();

    vao_bind(&p->vao);
    vbo_bind(&p->vbo);
    vbo_bind(&p->vio);
    vao_attribute(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    vao_attribute(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    vao_attribute(2, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(5 * sizeof(float)));

    vao_bind(NULL);

    return p;
}

void emit_face(chunk_t *p, int x, int y, int z, direction_e d) {
    // emit vertices
    for(int i = 0; i < 4; ++i) {
        const float *v = &CUBE_VERTICES[CUBE_INDICES[d * 6 + i] * 3];

        p->vertices[p->mesh_counter++] = x + v[0];
        p->vertices[p->mesh_counter++] = y + v[1];
        p->vertices[p->mesh_counter++] = z + v[2];
        p->vertices[p->mesh_counter++] = TEX_UV[i * 2];
        p->vertices[p->mesh_counter++] = TEX_UV[i * 2 + 1];

        if(y == CHUNK_SIZE_Y - 1) {
            if(d == TOP) p->vertices[p->mesh_counter++] = 1;
            else if(d == BOTTOM) p->vertices[p->mesh_counter++] = 2;
            else p->vertices[p->mesh_counter++] = 0;
        } else if(y == CHUNK_SIZE_Y - 2) {
            p->vertices[p->mesh_counter++] = 2;
        } else {
            p->vertices[p->mesh_counter++] = 3;
        }
        
    }

    // emit indices
    for(int i = 0; i < 6; ++i) {
        p->indices[p->index_count++] = p->vertex_count + CUBE_INDICES[i];
    }

    p->vertex_count += 4;
}

uint8_t get_chunk_block(chunk_t *p, int x, int y, int z) {
    if(x < 0 || x >= CHUNK_SIZE_X || y < 0 || y >= CHUNK_SIZE_Y || -z < 0 || -z >= CHUNK_SIZE_Z) {
        return 0; // TODO replace with data from other chunk
    }

    return *(p->data + (y * CHUNK_SIZE_X * CHUNK_SIZE_Z) + (-z * CHUNK_SIZE_X) + x);
}

void set_chunk_block(chunk_t *p, int x, int y, int z, uint8_t type) {
    if(x < 0 || x >= CHUNK_SIZE_X || y < 0 || y >= CHUNK_SIZE_Y || -z < 0 || -z >= CHUNK_SIZE_Z) {
        return; // TODO replace with data from other chunk
    }

    p->data[y * CHUNK_SIZE_X * CHUNK_SIZE_Z + -z * CHUNK_SIZE_X + x] = type;
}

void prepare_chunk(chunk_t *p) {
    uint8_t *d = p->data;
    p->mesh_counter = 0;
    p->index_count  = 0;
    p->vertex_count = 0;

    for(int y = 0; y < CHUNK_SIZE_Y; ++y) {
        for(int z = 0; z < CHUNK_SIZE_Z; ++z) {
            for(int x = 0; x < CHUNK_SIZE_X; ++x) {
                if(*d == 0) {
                    ++d;
                    continue;
                }

                if(!get_chunk_block(p, x, y, -z + 1)) emit_face(p, x, y, -z, 0);
                if(!get_chunk_block(p, x, y, -z - 1)) emit_face(p, x, y, -z, 1);
                if(!get_chunk_block(p, x + 1, y, -z)) emit_face(p, x, y, -z, 2);
                if(!get_chunk_block(p, x - 1, y, -z)) emit_face(p, x, y, -z, 3);
                if(!get_chunk_block(p, x, y + 1, -z)) emit_face(p, x, y, -z, 4);
                if(!get_chunk_block(p, x, y - 1, -z)) emit_face(p, x, y, -z, 5);
                ++d;
            }
        }
    }

    vbo_bind(&p->vbo);
    vbo_data(&p->vbo, p->mesh_counter * sizeof(float), p->vertices);
    vbo_bind(&p->vio);
    vbo_data(&p->vio, p->index_count * sizeof(GLuint), p->indices);
}

void chunk_render(chunk_t *p) {
    vao_bind(&p->vao);
    glDrawElements(GL_TRIANGLES, p->index_count, GL_UNSIGNED_INT, (void*)0);
}

void free_chunk(chunk_t *p) {
    if(!p) return;

    vbo_destroy(&p->vbo);
    vbo_destroy(&p->vio);
    vao_destroy(&p->vao);

    free(p->vertices);
    free(p->indices);

    free(p->data);
    free(p);
}