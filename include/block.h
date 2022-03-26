#ifndef __BLOCK_H__
#define __BLOCK_H__

#include <stdint.h>

#include "constants.h"

typedef enum {
    BLOCK_AIR = 0,
    BLOCK_GRASS = 1,
    BLOCK_DIRT = 2,
    BLOCK_STONE = 3,
    BLOCK_COBBLE = 4,
    BLOCK_PLANK = 5,
    BLOCK_GRANIT = 6,
    BLOCK_ROSE = 7,
    BLOCK_MAX_ID = 7
} blockid_e;

typedef struct {
    uint8_t ID;

    uint8_t (*get_texture_face)(direction_e);
} block_t;

void blocks_init();

extern block_t BLOCKS[BLOCK_MAX_ID + 1];

static inline char* block_name_from_id(uint8_t ID) {
    switch(ID) {
        case BLOCK_AIR: return "Air";
        case BLOCK_GRASS: return "Grass";
        case BLOCK_DIRT: return "Dirt";
        case BLOCK_STONE: return "Stone";
        case BLOCK_COBBLE: return "Cobble";
        case BLOCK_PLANK: return "Plank";
        case BLOCK_GRANIT: return "Granit";
        case BLOCK_ROSE: return "Rose";
        default: return "Undefined";
    }
}

#endif