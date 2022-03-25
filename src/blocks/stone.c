#include "block.h"

static inline uint8_t get_texture_face(direction_e d) {
    return 3;
}

void stone_init() {
    block_t p = {
        .ID = BLOCK_STONE,
        .get_texture_face = get_texture_face
    };

    BLOCKS[BLOCK_STONE] = p;
}