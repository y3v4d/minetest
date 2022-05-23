#include "block.h"

static inline uint8_t get_texture_face(direction_e d) {
    return 7;
}

void granit_init() {
    block_t p = {
        .ID = BLOCK_GRANIT,
        .is_transparent = FALSE,
        .get_texture_face = get_texture_face
    };

    BLOCKS[BLOCK_GRANIT] = p;
}