#include "block.h"

extern void grass_init();
extern void dirt_init();
extern void stone_init();

void blocks_init() {
    grass_init();
    dirt_init();
    stone_init();
}

block_t BLOCKS[BLOCK_MAX_ID + 1];