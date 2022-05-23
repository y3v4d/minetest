#include "block.h"

static inline uint8_t get_texture_face(direction_e d) {
    return 10;
}

void glass_init() {
    block_t p = {
        .ID = BLOCK_GLASS,
        .is_transparent = TRUE,
        .get_texture_face = get_texture_face
    };

    BLOCKS[BLOCK_GLASS] = p;
}
