#ifndef __EVENT_H__
#define __EVENT_H__

#include <stdint.h>

#define EVENT_UNDEFINED     0x00
#define EVENT_KEY_PRESS     0x01
#define EVENT_KEY_RELEASE   0x02
#define EVENT_MOUSE_MOVE    0x03
#define EVENT_MOUSE_PRESSED 0x04
#define EVENT_WINDOW_CLOSE  0x10
#define EVENT_WINDOW_RESIZE 0x11

#define MOUSE_BUTTON_0 0x00
#define MOUSE_BUTTON_1 0x01
#define MOUSE_BUTTON_2 0x02
#define MOUSE_BUTTON_3 0x03

typedef struct _event_mouse_s {
    unsigned type;
    int x, y;
    unsigned button;
} event_mouse_t;

typedef struct _event_key_s {
    unsigned type;
    char key;
} eventkey_t;

typedef struct _eventwindow_s {
    unsigned type;
    int width, height;
} eventwindow_t;

typedef union _event_s {
    unsigned type;
    eventkey_t eventkey;
    event_mouse_t eventmouse;
    eventwindow_t window;
} event_t;

#endif
