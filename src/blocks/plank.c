#include "block.h"

static inline uint8_t get_texture_face(direction_e d) {
    return 6;
}

void plank_init() {
    block_t p = {
        .ID = BLOCK_PLANK,
        .is_transparent = FALSE,
        .get_texture_face = get_texture_face
    };

    BLOCKS[BLOCK_PLANK] = p;
}