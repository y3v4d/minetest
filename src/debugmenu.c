#include "debugmenu.h"

#include "utils/vec.h"
#include "state.h"

#include <stdlib.h>
#include <stdio.h>

debugmenu_t* debugmenu_init(fontbmp_t *font) {
    debugmenu_t *temp = (debugmenu_t*)malloc(sizeof(debugmenu_t));
    if(!temp) {
        fprintf(stderr, "Couldn't allocate memory for debug menu.\n");
        return NULL;
    }

    temp->font = font;

    if(!(temp->fps = text_make(font, "FPS: ", F2VEC3F(0.f, 0.f, 0.f)))) 
        goto init_error;
    if(!(temp->block = text_make(font, "Block: Grass", F2VEC3F(0.f, 32.f, 0.f)))) 
        goto init_error;
    if(!(temp->mode = text_make(font, "Mode: Default", F2VEC3F(0.f, 64.f, 0.f)))) 
        goto init_error;
    if(!(temp->cam_mode = text_make(font, "Camera Mode: FILL", F2VEC3F(0.f, 96.f, 0.f)))) 
        goto init_error;
    if(!(temp->looking = text_make(font, "Looking at", F2VEC3F(0.f, state.window_height - 32.f, 0.f)))) 
        goto init_error;
    if(!(temp->position = text_make(font, "Pos", F2VEC3F(0.f, state.window_height - 96.f, 0.f)))) 
        goto init_error;
    if(!(temp->rotation = text_make(font, "Rot", F2VEC3F(0.f, state.window_height - 64.f, 0.f)))) 
        goto init_error;

    return temp;

init_error:
    debugmenu_destroy(temp);
    return NULL;
}

void debugmenu_destroy(debugmenu_t *p) {
    if(!p) return;

    if(p->fps) text_destroy(p->fps);
    if(p->block) text_destroy(p->block);
    if(p->mode) text_destroy(p->mode);
    if(p->cam_mode) text_destroy(p->cam_mode);
    if(p->looking) text_destroy(p->looking);
    if(p->position) text_destroy(p->position);
    if(p->rotation) text_destroy(p->rotation);

    free(p);
}

void debugmenu_update(debugmenu_t *dmenu) {
    snprintf(
            dmenu->position->text, 32, 
            "Pos %.2f %.2f %.2f", 
            state.world->player->position.x, 
            state.world->player->position.y, 
            state.world->player->position.z
    );


    snprintf(
            dmenu->rotation->text, 32, 
            "Rot %.2f %.2f", 
            state.world->player->rotation.x, 
            state.world->player->rotation.y
    );

    text_update(dmenu->position, TEXT_UPDATE_STRING);
    text_update(dmenu->rotation, TEXT_UPDATE_STRING);
}

void debugmenu_render(debugmenu_t *dmenu) {
    shader_use(state.shader_ui);

    text_render(dmenu->position, state.shader_ui);
    text_render(dmenu->rotation, state.shader_ui);
}
