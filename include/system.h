#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include "event.h"

#include <windows.h>

void g_init(HINSTANCE hInstance);
void g_close();

void g_swap_buffers();

int g_check_event(event_t *event);

int g_pending_events();
void g_get_event(event_t *event);

void g_lock_mouse();

void g_end_frame();

extern HWND w_hwnd;

#endif
