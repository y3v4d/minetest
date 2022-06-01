#include "sys/system.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include <GL/glew.h>
#include <GL/glx.h>

void GLAPIENTRY
MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    if(type == GL_DEBUG_TYPE_ERROR) {
        fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity, message);
    }
}

typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);

// variables neccessary for the window
Display *display;
Window window;
int screen;
GLXContext context;

XEvent x_event;

Cursor xcursor;

// atoms
Atom wm_delete_window;

Bool x_expose_occurred = False;

int x_last_width = -1;
int x_last_height = -1;

void g_init(int width, int height) {
    // open new display connection
    display = XOpenDisplay(NULL);
    if(!display) {
        printf("Failed to open X display\n");
        exit(1);
    }

    int screen = XDefaultScreen(display);

    // preffered visual attributes for the framebuffer
    int visual_attribs[] = {
        GLX_X_RENDERABLE, True,
        GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
        GLX_RENDER_TYPE, GLX_RGBA_BIT,
        GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
        GLX_RED_SIZE, 8,
        GLX_GREEN_SIZE, 8,
        GLX_BLUE_SIZE, 8,
        GLX_ALPHA_SIZE, 8,
        GLX_DEPTH_SIZE, 24,
        GLX_STENCIL_SIZE, 8,
        GLX_DOUBLEBUFFER, True,
        None
    };

    printf("Getting matching framebuffer configs\n");

    int fb_count;
    GLXFBConfig *fbc = glXChooseFBConfig(display, screen, visual_attribs, &fb_count);
    if(!fbc) {
        printf("Failed to retrive framebuffer config\n");
        exit(1);
    }

    printf("Found %d matching configs\n", fb_count);

    // get the best config possible (look at the amount of samples)
    int best_samples_num = -1, best_index = -1;
    for(int i = 0; i < fb_count; ++i) {
        XVisualInfo *vi = glXGetVisualFromFBConfig(display, fbc[i]);
        if(vi) {
            int sample_buffers, samples;

            glXGetFBConfigAttrib(display, fbc[i], GLX_SAMPLE_BUFFERS, &sample_buffers);
            glXGetFBConfigAttrib(display, fbc[i], GLX_SAMPLES, &samples);

            printf("Matching framebuffer config %d, visual ID %ld: SAMPLE_BUFFERS = %d, SAMPLES = %d\n", i, vi->visualid, sample_buffers, samples);

            if(sample_buffers && samples > best_samples_num) {
                best_index = i;
                best_samples_num = samples;
            }
        }

        XFree(vi);
    }

    GLXFBConfig best_fb = fbc[best_index];
    XFree(fbc);

    // get visual info from the framebuffer config
    XVisualInfo *vi = glXGetVisualFromFBConfig(display, best_fb);
    if(!vi) {
        printf("Couldn't get visual info from framebuffer config\n");
        exit(1);
    }

    printf("Chosen visual ID %ld\n", vi->visualid);

    // set window attributes
    XSetWindowAttributes window_attributes;
    window_attributes.colormap = XCreateColormap(display, XRootWindow(display, vi->screen), vi->visual, AllocNone);
    window_attributes.background_pixel = XBlackPixel(display, vi->screen);
    window_attributes.border_pixel = XBlackPixel(display, vi->screen);
    window_attributes.event_mask = KeyPressMask | KeyReleaseMask | PointerMotionMask | StructureNotifyMask | ButtonPressMask;

    // create window
    window = XCreateWindow(
            display,                                                    // display
            XRootWindow(display, vi->screen),                           // root window
            0, 0,                                                       // window position
            width, height,                                              // window size
            0,                                                          // border size
            vi->depth,                                                  // depth
            InputOutput,                                                // window flags
            vi->visual,                                                 // visual
            CWBackPixel | CWBorderPixel | CWColormap | CWEventMask,     // attributes flags
            &window_attributes                                          // window attributes
    );
    if(!window) {
        printf("Failed to create X window\n");
        exit(1);
    }

    XStoreName(display, window, "MineTest - X11 OpenGL 4.x");

    // free visual info
    XFree(vi);

    // setup WM_DELETE_WINDOW atom
    wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &wm_delete_window, 1);

    // setup WM_CLASS
    XClassHint *class_hint = XAllocClassHint();

    class_hint->res_name = "Minetest";
    class_hint->res_class = "minetest";

    XSetClassHint(display, window, class_hint);

    // show window
    XMapWindow(display, window);

    // get ARB function to create modern opengl context in x11
    glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
    glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddressARB((const GLubyte*)"glXCreateContextAttribsARB");

    // create opengl context
    GLXContext context = 0;
    if(!glXCreateContextAttribsARB) {
        printf("Can't create modern OpenGL context, not supported\n");
        exit(1);
    } else {
        int context_attribs[] = {
            GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
            GLX_CONTEXT_MINOR_VERSION_ARB, 0,
            None
        };

        context = glXCreateContextAttribsARB(display, best_fb, 0, True, context_attribs);
        if(!context) {
            printf("Can't create modern OpenGL context\n");
            exit(1);
        }
    }

    // make newly created opengl context current
    glXMakeCurrent(display, window, context);

    // start glew
    glewExperimental = GL_TRUE;
    glewInit();

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);

    // make invisible cursor (junky ass shit)
    Pixmap cursor_pixmap = XCreateBitmapFromData(display, window, (const char[]){ 0 }, 1, 1);
    XColor color = {
        .flags = 0
    };
    XQueryColor(display, XDefaultColormap(display, screen), &color);
    xcursor = XCreatePixmapCursor(display, cursor_pixmap, cursor_pixmap, &color, &color, 0, 0);

    XDefineCursor(display, window, xcursor);
    XFreePixmap(display, cursor_pixmap);
}

Bool mouse_warped = False;
void g_lock_mouse() {
    XWarpPointer(display, None, window, 0, 0, 0, 0, 320, 240);
    mouse_warped = True;
}

void g_swap_buffers() {
    glXSwapBuffers(display, window);
}

void g_close() {
    //XFreeCursor(display, xcursor);

    glXMakeCurrent(display, 0, 0);
    glXDestroyContext(display, context);

    XDestroyWindow(display, window);
    XCloseDisplay(display);
}

int g_pending_events() {
    return XPending(display);
}

unsigned recent_button = 0;

void g_get_event(event_t *event) {
    XNextEvent(display, &x_event);

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
            if(x_last_width != x_event.xconfigure.width || x_last_height != x_event.xconfigure.height) {
                // resize happened
                x_last_width = x_event.xconfigure.width;
                x_last_height = x_event.xconfigure.height;

                event->type = EVENT_WINDOW_RESIZE;
                event->window.width = x_last_width;
                event->window.height = x_last_height;

                glViewport(0, 0, x_last_width, x_last_height);
            } else {
                event->type = EVENT_UNDEFINED;
            }
            
            break;
        case ClientMessage:
            if(x_event.xclient.data.l[0] == (long)wm_delete_window) {
                event->type = EVENT_WINDOW_CLOSE;
            }
            break;
        default: event->type = EVENT_UNDEFINED; break;
    }
}
