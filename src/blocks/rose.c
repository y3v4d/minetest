#include "block.h"

static inline uint8_t get_texture_face(direction_e d) {
    switch(d) {
        case TOP: case BOTTOM: return 8;
        default: return 8;
    }
}

void rose_init() {
    block_t p = {
        .ID = BLOCK_ROSE,
        .get_texture_face = get_texture_face
    };

    BLOCKS[BLOCK_ROSE] = p;
}