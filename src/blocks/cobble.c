#include "block.h"

static inline uint8_t get_texture_face(direction_e d) {
    return 5;
}

void cobble_init() {
    block_t p = {
        .ID = BLOCK_COBBLE,
        .get_texture_face = get_texture_face
    };

    BLOCKS[BLOCK_COBBLE] = p;
}