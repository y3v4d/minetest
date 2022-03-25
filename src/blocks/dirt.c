#include "block.h"

static inline uint8_t get_texture_face(direction_e d) {
    return 2;
}

void dirt_init() {
    block_t p = {
        .ID = BLOCK_DIRT,
        .get_texture_face = get_texture_face
    };

    BLOCKS[BLOCK_DIRT] = p;
}