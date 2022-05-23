#include "block.h"

static inline uint8_t get_texture_face(direction_e d) {
    switch(d) {
        case TOP: return 1;
        case BOTTOM: return 2;
        default: return 0;
    }
}

void grass_init() {
    block_t p = {
        .ID = BLOCK_GRASS,
        .is_transparent = FALSE,
        .get_texture_face = get_texture_face
    };

    BLOCKS[BLOCK_GRASS] = p;
}