#include "block.h"

static inline uint8_t get_texture_face(direction_e d) {
    return 9;
}

void wood_init() {
    block_t p = {
        .ID = BLOCK_WOOD,
        .get_texture_face = get_texture_face
    };

    BLOCKS[BLOCK_WOOD] = p;
}