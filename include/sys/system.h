#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include "sys/event.h"

void g_init(int width, int height);
void g_close();

void g_swap_buffers();

int g_pending_events();
void g_get_event(event_t *event);

void g_lock_mouse();

void g_end_frame();

#endif
