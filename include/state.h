#ifndef __STATE_H__
#define __STATE_H__

#include "world.h"
#include "glx/atlas.h"
#include "glx/shader.h"

typedef struct {
    world_t *world;
    atlas_t *atlas;

    shader_t *shader_main;
    shader_t *shader_ui;

    int window_width, window_height;
} state_t;

extern state_t state;

#endif
