#ifndef __EVENT_H__
#define __EVENT_H__

#define EVENT_UNDEFINED     0x00
#define EVENT_KEY_PRESS     0x01
#define EVENT_KEY_RELEASE   0x02
#define EVENT_MOUSE_MOVE    0x03
#define EVENT_WINDOW_CLOSE  0x10

typedef struct _event_mouse_s {
    unsigned type;
    int x, y;
} event_mouse_t;

typedef struct _event_key_s {
    unsigned type;
    char key;
} eventkey_t;

typedef union _event_s {
    unsigned type;
    eventkey_t eventkey;
    event_mouse_t eventmouse;
} event_t;

#endif
