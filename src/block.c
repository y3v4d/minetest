#include "block.h"

extern void grass_init();
extern void dirt_init();
extern void stone_init();
extern void cobble_init();
extern void plank_init();
extern void granit_init();
extern void rose_init();

void blocks_init() {
    grass_init();
    dirt_init();
    stone_init();
    cobble_init();
    plank_init();
    granit_init();
    rose_init();
}

block_t BLOCKS[BLOCK_MAX_ID + 1];