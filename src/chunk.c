#include "chunk.h"
#include "world.h"
#include "block.h"
#include "math/matrix.h"

#include "direction.h"

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

chunk_t *chunk_init(world_t *world, int x, int z) {
    chunk_t *p = (chunk_t*)malloc(sizeof(chunk_t));
    memset(p, 0, sizeof(chunk_t));

    p->world = world;

    p->position.x = x;
    p->position.y = z;

    const size_t SIZE = CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z;
    p->data = (uint8_t*)malloc(sizeof(uint8_t) * SIZE);

    p->meshes.base = mesh_init();
    p->meshes.transparent = mesh_init();

    /*memset(p->data, BLOCK_STONE, 4 * CHUNK_SIZE_X * CHUNK_SIZE_Z);
    memset(p->data + 4 * CHUNK_SIZE_X * CHUNK_SIZE_Z, BLOCK_DIRT, CHUNK_SIZE_X * CHUNK_SIZE_Z);
    memset(p->data + 5 * CHUNK_SIZE_X * CHUNK_SIZE_Z, BLOCK_GRASS, CHUNK_SIZE_X * CHUNK_SIZE_Z);
    memset(p->data + 6 * CHUNK_SIZE_X * CHUNK_SIZE_Z, BLOCK_AIR, SIZE - 6 * CHUNK_SIZE_X * CHUNK_SIZE_Z);*/

    memset(p->data, BLOCK_PLANK, 6 * CHUNK_SIZE_X * CHUNK_SIZE_Z);
    memset(p->data + 6 * CHUNK_SIZE_X * CHUNK_SIZE_Z, BLOCK_AIR, SIZE - 6 * CHUNK_SIZE_X * CHUNK_SIZE_Z);

    for(int y = 0; y < 6; ++y) {
        for(int x = 0; x < CHUNK_SIZE_X; ++x) {
            p->data[y * CHUNK_SIZE_X * CHUNK_SIZE_Z + x] = BLOCK_WOOD;
            p->data[y * CHUNK_SIZE_X * CHUNK_SIZE_Z + 15 * CHUNK_SIZE_Z + x] = BLOCK_WOOD;
        }

        for(int z = 0; z < CHUNK_SIZE_Z; ++z) {
            p->data[y * CHUNK_SIZE_Z * CHUNK_SIZE_X + CHUNK_SIZE_X * z] = BLOCK_WOOD;
            p->data[y * CHUNK_SIZE_Z * CHUNK_SIZE_X + 15 + CHUNK_SIZE_X * z] = BLOCK_WOOD;
        }
    }

    return p;
}

void chunk_destroy(chunk_t *p) {
    if(!p) return;

    mesh_destroy(p->meshes.base);
    mesh_destroy(p->meshes.transparent);

    if(p->data) free(p->data);
    free(p);
}

void emit_face(chunk_t *p, int x, int y, int z, direction_e d, uint8_t block_id) {
    mesh_t *mesh = (BLOCKS[block_id].is_transparent ? p->meshes.transparent : p->meshes.base);

    // emit vertices
    for(int i = 0; i < 4; ++i) {
        const float *v = &CUBE_VERTICES[CUBE_INDICES[d * 6 + i] * 3];

        ((float*)mesh->vertices->data)[mesh->vertex_counter++] = x + v[0];
        ((float*)mesh->vertices->data)[mesh->vertex_counter++] = y + v[1];
        ((float*)mesh->vertices->data)[mesh->vertex_counter++] = z + v[2];
        ((float*)mesh->vertices->data)[mesh->vertex_counter++] = TEX_UV[i * 2];
        ((float*)mesh->vertices->data)[mesh->vertex_counter++] = TEX_UV[i * 2 + 1];

        ((float*)mesh->vertices->data)[mesh->vertex_counter++] = BLOCKS[block_id].get_texture_face(d);
    }

    // emit indices
    for(int i = 0; i < 6; ++i) {
        ((int*)mesh->indices->data)[mesh->indices->index++] = mesh->vertices->index + CUBE_INDICES[i];
    }
    
    mesh->vertices->index += 4;
}

bool_e is_block_in_bounds(const chunk_t *p, int x, int y, int z) {
    return x >= 0 && x < CHUNK_SIZE_X && y >= 0 && y < CHUNK_SIZE_Y && -z > 0 && -z < CHUNK_SIZE_Z;
}

bool_e is_on_boundry(const chunk_t *p, int x, int y, int z) {
    return x == 0 || x == CHUNK_SIZE_X - 1 || -z == 0 || -z == CHUNK_SIZE_Z - 1;
}

uint8_t chunk_get_block(const chunk_t *p, int x, int y, int z) {
    if(x < 0 || x >= CHUNK_SIZE_X || y < 0 || y >= CHUNK_SIZE_Y || -z < 0 || -z >= CHUNK_SIZE_Z) {
        return 0;
    }

    return *(p->data + (y * CHUNK_SIZE_X * CHUNK_SIZE_Z) + (-z * CHUNK_SIZE_X) + x);
}

void chunk_set_block(chunk_t *p, int x, int y, int z, uint8_t type) {
    if(x < 0 || x >= CHUNK_SIZE_X || y < 0 || y >= CHUNK_SIZE_Y || -z < 0 || -z >= CHUNK_SIZE_Z) {
        return;
    }

    const int coord = y * CHUNK_SIZE_X * CHUNK_SIZE_Z + -z * CHUNK_SIZE_X + x;

    if(y > 0 && p->data[coord - CHUNK_SIZE_X * CHUNK_SIZE_Z] == BLOCK_GRASS) {
        p->data[coord - CHUNK_SIZE_X * CHUNK_SIZE_Z] = BLOCK_DIRT;
    }

    p->data[coord] = type;

    if(is_on_boundry(p, x, y, z)) {
        chunk_t *n[2] = { NULL, NULL };

        int i = 0;
        if(x == 0) {
            n[i++] = world_find_chunk(p->world, p->position.x - 1, p->position.y);
        } else if(x == CHUNK_SIZE_X - 1) {
            n[i++] = world_find_chunk(p->world, p->position.x + 1, p->position.y);
        }

        if(z == 0) {
            n[i++] = world_find_chunk(p->world, p->position.x, p->position.y + 1);
        } else if(-z == CHUNK_SIZE_Z - 1) {
            n[i++] = world_find_chunk(p->world, p->position.x, p->position.y - 1);
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

    mesh_prepare(p->meshes.base);
    mesh_prepare(p->meshes.transparent);

    for(int y = 0; y < CHUNK_SIZE_Y; ++y) {
        for(int z = 0; z < CHUNK_SIZE_Z; ++z) {
            for(int x = 0; x < CHUNK_SIZE_X; ++x) {
                if(*b == BLOCK_AIR) {
                    ++b;
                    continue;
                }

                for(direction_e d = 0; d < 6; ++d) {
                    vec3i dv = DIR2VEC3I(d);
                    vec3i neighbor = { x + dv.x, y + dv.y, z + dv.z };
                    vec3i wneighbor = {
                        p->position.x * CHUNK_SIZE_X + x + dv.x,
                        y + dv.y,
                        -p->position.y * CHUNK_SIZE_Z + z + dv.z
                    };

                    block_t neighbor_block = BLOCKS[BLOCK_AIR];

                    if(is_block_in_bounds(p, neighbor.x, neighbor.y, -neighbor.z)) {
                        neighbor_block = BLOCKS[chunk_get_block(p, neighbor.x, neighbor.y, -neighbor.z)];
                    } else {
                        neighbor_block = BLOCKS[world_get_block(p->world, wneighbor.x, wneighbor.y, -wneighbor.z)];
                    }

                    if(neighbor_block.ID == BLOCK_AIR || neighbor_block.is_transparent == TRUE) {
                        emit_face(p, x, y, -z, d, *b);
                    }
                }

                ++b;
            }
        }
    }

    mesh_finalize(p->meshes.base);
    mesh_finalize(p->meshes.transparent);
}

void chunk_render(chunk_t *p, shader_t *s) {
    mat4_t model = mat4_translation(p->position.x * CHUNK_SIZE_X, 0, p->position.y * CHUNK_SIZE_Z);
    shader_uniform(s, "model", UNIFORM_MATRIX_4, 1, model.m);

    vao_bind(&p->meshes.base->vao);
    glDrawElements(GL_TRIANGLES, p->meshes.base->indices->index, GL_UNSIGNED_INT, (void*)0);

    vao_bind(&p->meshes.transparent->vao);
    glDrawElements(GL_TRIANGLES, p->meshes.transparent->indices->index, GL_UNSIGNED_INT, (void*)0);
}