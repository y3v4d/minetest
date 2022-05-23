#include "chunk.h"
#include "world.h"
#include "block.h"
#include "constants.h"
#include "math/matrix.h"

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

vec3i dir_to_vec3i(direction_e d) {
    switch(d) {
        case FRONT: return (vec3i) { 0, 0, -1 };
        case BACK: return (vec3i) { 0, 0, 1 };
        case RIGHT: return (vec3i) { 1, 0, 0 };
        case LEFT: return (vec3i) { -1, 0, 0 };
        case TOP: return (vec3i) { 0, 1, 0 };
        case BOTTOM: return (vec3i) { 0, -1, 0 };
        default: return (vec3i) { 0, 0, 0 };
    }
}

chunk_t *initialize_chunk(world_t *world, int x, int z) {
    chunk_t *p = (chunk_t*)malloc(sizeof(chunk_t));
    memset(p, 0, sizeof(chunk_t));

    p->is_dirty = FALSE;
    p->world = world;

    p->x = x;
    p->z = z;

    const size_t SIZE = CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z;
    p->data = (uint8_t*)malloc(sizeof(uint8_t) * SIZE);

    p->vertices = (float*)malloc(sizeof(float) * SIZE * 6 * 4 * 6);
    p->indices = (int*)malloc(sizeof(int) * SIZE * 6 * 6);

    //memset(p->data, BLOCK_AIR, SIZE);
    //memset(p->data, BLOCK_DIRT, 1);

    memset(p->data, BLOCK_STONE, 4 * CHUNK_SIZE_X * CHUNK_SIZE_Z);
    memset(p->data + 4 * CHUNK_SIZE_X * CHUNK_SIZE_Z, BLOCK_DIRT, CHUNK_SIZE_X * CHUNK_SIZE_Z);
    memset(p->data + 5 * CHUNK_SIZE_X * CHUNK_SIZE_Z, BLOCK_GRASS, CHUNK_SIZE_X * CHUNK_SIZE_Z);
    memset(p->data + 6 * CHUNK_SIZE_X * CHUNK_SIZE_Z, BLOCK_AIR, SIZE - 6 * CHUNK_SIZE_X * CHUNK_SIZE_Z);

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

void emit_face(chunk_t *p, int x, int y, int z, direction_e d, uint8_t block_id) {
    // emit vertices
    for(int i = 0; i < 4; ++i) {
        const float *v = &CUBE_VERTICES[CUBE_INDICES[d * 6 + i] * 3];

        p->vertices[p->mesh_counter++] = x + v[0];
        p->vertices[p->mesh_counter++] = y + v[1];
        p->vertices[p->mesh_counter++] = z + v[2];
        p->vertices[p->mesh_counter++] = TEX_UV[i * 2];
        p->vertices[p->mesh_counter++] = TEX_UV[i * 2 + 1];

        p->vertices[p->mesh_counter++] = BLOCKS[block_id].get_texture_face(d);
    }

    // emit indices
    for(int i = 0; i < 6; ++i) {
        p->indices[p->index_count++] = p->vertex_count + CUBE_INDICES[i];
    }

    p->vertex_count += 4;
}

bool_e is_block_in_bounds(const chunk_t *p, int x, int y, int z) {
    return x >= 0 && x < CHUNK_SIZE_X && y >= 0 && y < CHUNK_SIZE_Y && -z > 0 && -z < CHUNK_SIZE_Z;
}

bool_e is_on_boundry(const chunk_t *p, int x, int y, int z) {
    return x == 0 || x == CHUNK_SIZE_X - 1 || -z == 0 || -z == CHUNK_SIZE_Z - 1;
}

uint8_t get_chunk_block(const chunk_t *p, int x, int y, int z) {
    if(x < 0 || x >= CHUNK_SIZE_X || y < 0 || y >= CHUNK_SIZE_Y || -z < 0 || -z >= CHUNK_SIZE_Z) {
        return 0; // TODO replace with data from other chunk
    }

    return *(p->data + (y * CHUNK_SIZE_X * CHUNK_SIZE_Z) + (-z * CHUNK_SIZE_X) + x);
}

void set_chunk_block(chunk_t *p, int x, int y, int z, uint8_t type) {
    if(x < 0 || x >= CHUNK_SIZE_X || y < 0 || y >= CHUNK_SIZE_Y || -z < 0 || -z >= CHUNK_SIZE_Z) {
        return; // TODO replace with data from other chunk
    }

    const int coord = y * CHUNK_SIZE_X * CHUNK_SIZE_Z + -z * CHUNK_SIZE_X + x;

    if(y > 0 && p->data[coord - CHUNK_SIZE_X * CHUNK_SIZE_Z] == BLOCK_GRASS) {
        p->data[coord - CHUNK_SIZE_X * CHUNK_SIZE_Z] = BLOCK_DIRT;
    }

    p->data[coord] = type;
    p->is_dirty = TRUE;

    if(is_on_boundry(p, x, y, z)) {
        chunk_t *n[2];

        int i = 0;
        if(x == 0) {
            n[i++] = world_find_chunk(p->world, p->x - 1, p->z);
        } else if(x == CHUNK_SIZE_X - 1) {
            n[i++] = world_find_chunk(p->world, p->x + 1, p->z);
        }

        if(z == 0) {
            n[i++] = world_find_chunk(p->world, p->x, p->z + 1);
        } else if(-z == CHUNK_SIZE_Z - 1) {
            n[i++] = world_find_chunk(p->world, p->x, p->z - 1);
        }

        for(int i = 0; i < 2; ++i) {
            if(n[i] != NULL) {
                prepare_chunk(n[i]);
            }
        }
    }
}

void prepare_chunk(chunk_t *p) {
    uint8_t *b = p->data;

    p->mesh_counter = 0;
    p->index_count  = 0;
    p->vertex_count = 0;

    for(int y = 0; y < CHUNK_SIZE_Y; ++y) {
        for(int z = 0; z < CHUNK_SIZE_Z; ++z) {
            for(int x = 0; x < CHUNK_SIZE_X; ++x) {
                if(*b == BLOCK_AIR) {
                    ++b;
                    continue;
                }

                for(direction_e d = 0; d < 6; ++d) {
                    vec3i dv = dir_to_vec3i(d);
                    vec3i neighbor = { x + dv.x, y + dv.y, z + dv.z };
                    vec3i wneighbor = {
                        p->x * CHUNK_SIZE_X + x + dv.x,
                        y + dv.y,
                        -p->z * CHUNK_SIZE_Z + z + dv.z
                    };

                    block_t neighbor_block = BLOCKS[BLOCK_AIR];

                    if(is_block_in_bounds(p, neighbor.x, neighbor.y, -neighbor.z)) {
                        neighbor_block = BLOCKS[get_chunk_block(p, neighbor.x, neighbor.y, -neighbor.z)];
                    } else {
                        neighbor_block = BLOCKS[world_get_block(p->world, wneighbor.x, wneighbor.y, -wneighbor.z)];
                    }

                    if(neighbor_block.ID == BLOCK_AIR || neighbor_block.is_transparent == TRUE) {
                        emit_face(p, x, y, -z, d, *b);
                    }

                    /*if(is_block_in_bounds(p, neighbor.x, neighbor.y, -neighbor.z)) {
                        if(!get_chunk_block(p, neighbor.x, neighbor.y, -neighbor.z)) emit_face(p, x, y, -z, d, *b);
                    } else {
                        
                    }*/
                }

                ++b;
            }
        }
    }

    vbo_bind(&p->vbo);
    vbo_data(&p->vbo, p->mesh_counter * sizeof(float), p->vertices);
    vbo_bind(&p->vio);
    vbo_data(&p->vio, p->index_count * sizeof(GLuint), p->indices);
}

void chunk_render(chunk_t *p, shader_t *s) {
    mat4_t model = mat4_translation(p->x * CHUNK_SIZE_X, 0, p->z * CHUNK_SIZE_Z);
    shader_uniform(s, "model", UNIFORM_MATRIX_4, 1, model.m);

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

const char* direction_name(direction_e d) {
    switch(d) {
        case FRONT: return "FRONT";
        case BACK: return "BACK";
        case LEFT: return "LEFT";
        case RIGHT: return "RIGHT";
        case TOP: return "TOP";
        case BOTTOM: return "BOTTOM";
    }

    return NULL;
}

vec3f direction_to_vec3f(direction_e d) {
    vec3f temp = { 0, 0, 0 };

    switch(d) {
        case FRONT: temp.z = 1; break;
        case BACK: temp.z = -1; break;
        case LEFT: temp.x = -1; break;
        case RIGHT: temp.x = 1; break;
        case TOP: temp.y = 1; break;
        case BOTTOM: temp.y = -1; break;
    }

    return temp;
}