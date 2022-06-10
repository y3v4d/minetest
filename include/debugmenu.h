#ifndef __DEBUG_MENU_H__
#define __DEBUG_MENU_H__

#include "text.h"
#include "glx/shader.h"
#include "utils/font_loader.h"

typedef struct {
    fontbmp_t *font;

    text_t *fps;
    text_t *block;
    text_t *mode;
    text_t *cam_mode;
    text_t *looking;
    text_t *position;
    text_t *rotation;
} debugmenu_t;

debugmenu_t* debugmenu_init(fontbmp_t *font);
void debugmenu_destroy(debugmenu_t *p);

void debugmenu_update(debugmenu_t *dmenu);
void debugmenu_render(debugmenu_t *dmenu);

#endif
