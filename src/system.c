#include "system.h"
#include "constants.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <windows.h>

#include <GL/glew.h>
#include <GL/wglext.h>
#include <GL/gl.h>

typedef HGLRC (*wglCreateContextAttribsARBProc)(HDC, HGLRC, const int*);

HWND w_hwnd;
HDC w_hdc;
HGLRC w_context;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void g_init(HINSTANCE hInstance) {
    const wchar_t CLASS_NAME[] = L"win32-c";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    if(RegisterClass(&wc)  == 0) {
        DWORD code = GetLastError();
        fprintf(stderr, "Couldn't register class. Code %d\n", code);

        return 1;
    }

    w_hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"MineTest Win32 OpenGL",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 640, 480,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if(w_hwnd == NULL) {
        DWORD code = GetLastError();
        fprintf(stderr, "Couldn't create a window. Code: %d\n", code);

        return 1;
    }

    // dummy context for quering modern context extensions
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    //Flags
        PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
        32,                   // Colordepth of the framebuffer.
        8, 0, 8, 0, 8, 0,
        8,
        0,
        0,
        0, 0, 0, 0,
        24,                   // Number of bits for the depthbuffer
        8,                    // Number of bits for the stencilbuffer
        0,                    // Number of Aux buffers in the framebuffer.
        PFD_MAIN_PLANE,
        0,
        0, 0, 0
    };

    w_hdc = GetDC(w_hwnd);

    int format = ChoosePixelFormat(w_hdc, &pfd);
    SetPixelFormat(w_hdc, format, &pfd);

    HGLRC dummy = wglCreateContext(w_hdc);
    wglMakeCurrent(w_hdc, dummy);

    wglCreateContextAttribsARBProc wglCreateContextAttribsARB;
    wglCreateContextAttribsARB = (wglCreateContextAttribsARBProc)wglGetProcAddress("wglCreateContextAttribsARB");

    if(!wglCreateContextAttribsARB) {
        fprintf(stderr, "GL context >3.0 not supported\n");
        return 1;
    }

    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(dummy);

    printf("GL context >3.0 supported!\n");

    const int contextAttribList[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 0,
        WGL_CONTEXT_FLAGS_ARB, 0,
        0
    };

    w_context = wglCreateContextAttribsARB(w_hdc, 0, contextAttribList);

    wglMakeCurrent(w_hdc, w_context);

    // start glew
    glewExperimental = GL_TRUE;
    int glew_code = glewInit();
    if(glew_code != GLEW_OK) {
        fprintf(stderr, "glewInit error: %s\n", glewGetErrorString(glew_code));
        return 1;
    }

    const GLubyte *version = glGetString(GL_VERSION);
    printf("OpenGL version: %s\n", version);

    // make invisible cursor (junky ass shit)
}

//Bool mouse_warped = False;
void g_lock_mouse() {
    //XWarpPointer(display, None, window, 0, 0, 0, 0, 320, 240);
    //mouse_warped = True;
}

void g_swap_buffers() {
    SwapBuffers(w_hdc);
}

void g_close() {
    //XFreeCursor(display, xcursor);

    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(w_context);
}

int g_pending_events() {
    return 0;
    //return XPending(display);
}

unsigned recent_button = 0;

void g_get_event(event_t *event) {
    /*XNextEvent(display, &x_event);

    switch(x_event.type) {
        case KeyPress:
            event->type = EVENT_KEY_PRESS;
            event->eventkey.key = XLookupKeysym(&x_event.xkey, 0);
            break;
        case KeyRelease:
            event->type = EVENT_KEY_RELEASE;
            event->eventkey.key = XLookupKeysym(&x_event.xkey, 0);
            break;
        case ButtonPress:
            event->type = EVENT_MOUSE_PRESSED;
            event->eventmouse.x = x_event.xbutton.x;
            event->eventmouse.y = x_event.xbutton.y;
            event->eventmouse.button = x_event.xbutton.button;
            recent_button = x_event.xbutton.button;

            break;
        case MotionNotify:
            if(mouse_warped == True) { 
                event->type = EVENT_UNDEFINED;
                mouse_warped = False; 
            } else {
                event->type = EVENT_MOUSE_MOVE;
                event->eventmouse.x = x_event.xmotion.x;
                event->eventmouse.y = x_event.xmotion.y;
                event->eventmouse.button = recent_button;
            }
    
            break;
        case ConfigureNotify:
            glViewport(0, 0, x_event.xconfigure.width, x_event.xconfigure.height);
            break;
        case ClientMessage:
            if(x_event.xclient.data.l[0] == (long)wm_delete_window) {
                event->type = EVENT_WINDOW_CLOSE;
            }
            break;
        default: event->type = EVENT_UNDEFINED; break;
    }*/

    event->type = EVENT_UNDEFINED;
}
