#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <GL/glew.h>
#include <GL/glx.h>

#include "system.h"
#include "shader.h"
#include "event.h"

#include "utils/img_loader.h"

#include <math.h>

#include "atlas.h"

#include "math/vec.h"
#include "math/matrix.h"

#include "chunk.h"

void GLAPIENTRY
MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity, message);
}

int main() {
    g_init();

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEPTH_TEST);
    glDebugMessageCallback(MessageCallback, 0);

    const GLubyte *version = glGetString(GL_VERSION);
    printf("OpenGL version: %s\n", version);

    chunk_t *chunk = initialize_chunk();
    prepare_chunk(chunk);

    shader_t *shader = make_shader("data/shaders/main");
    if(!shader) {
        fprintf(stderr, "Error making shader main\n");
        return 1;
    }

    atlas_t *atlas = atlas_generate("data/textures/atlas.bmp", 16, 16);
    if(!atlas) {
        return 1;
    }

    mat4_t model = mat4_identity();
    mat4_t view = mat4_identity();
    mat4_t projection = mat4_perspective(45.f, 640.f / 480.f, 0.1f, 10.f);

    const float SPEED = 0.1f;
    const float ROTATION_SPEED = 0.001f;

    vec3f pos = { 0.f, 0.f, 0.f };
    vec3f vel = { 0.f, 0.f, 0.f };

    vec2f rot = { 0.f, 0.f };

    float m_x = 0.0f;
    float m_y = 0.0f;

    float pm_x = -1.0f;
    float pm_y = -1.0f;

    float dm_x = 0.0f;
    float dm_y = 0.0f;

    int move = 0;

    // event loop
    event_t event;
    short done = 0;
    while(!done) {
        while(g_pending_events()) {
            g_get_event(&event);

            dm_x = 0.0f;

            if(event.type == EVENT_KEY_PRESS) {
                char key = event.eventkey.key;

                switch(key) {
                    case 'q': done = 1; break;
                    case 'r':
                        close_shader(shader);
                        shader = make_shader("data/shaders/main");
                        break;
                    case 'w':
                        move = 1;
                        break;
                    case 's':
                        move = -1;
                        break;
                    case 'g':
                        chunk->data[2] = 0;
                        prepare_chunk(chunk);
                        break;
                    default: break;
                }
            } else if(event.type == EVENT_KEY_RELEASE) {
                char key = event.eventkey.key;

                if(key == 'w' || key == 's') {
                    move = 0;
                }
            } else if(event.type == EVENT_WINDOW_CLOSE) {
                printf("WM_DELETE_WINDOW invoked\n");
                done = 1;
            } else if(event.type == EVENT_MOUSE_MOVE) {
                m_x = event.eventmouse.x;
                m_y = event.eventmouse.y;
            }
        }

        dm_x = m_x - pm_x;
        dm_y = m_y - pm_y;

        rot.x += (-dm_y) / 5.f;
        rot.y += (-dm_x) / 5.f;

        if(rot.x > 90.f) rot.x = 90.f;
        else if(rot.x < -90.f) rot.x = -90.f;

        if(move != 0) {
            // Negative sin and cos where Y rotation
            // because the camera is looking at -z by default
            // (Y rotation responsible for the horizontal and depth movement)
            vel.x = -sinf(RADIANS(rot.y)) * cosf(RADIANS(rot.x)) * SPEED * move;
            vel.y = sinf(RADIANS(rot.x)) * SPEED * move;
            vel.z = -cosf(RADIANS(rot.y)) * cosf(RADIANS(rot.x)) * SPEED * move;
        } else {
            vel.x = 0;
            vel.y = 0;
            vel.z = 0;
        }

        pos.x += vel.x;
        pos.y += vel.y;
        pos.z += vel.z;

        g_lock_mouse();
        pm_x = 320; pm_y = 240;
        m_x = 320; m_y = 240;

        view = mat4_rotation_x(-rot.x);
        view = mat4_mul_mat4(view, mat4_rotation_y(-rot.y));
        view = mat4_mul_mat4(view, mat4_translation(-pos.x, -pos.y, -pos.z));

        glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader_use(shader);
        shader_uniform(shader, "model", UNIFORM_MATRIX_4, 1, model.m);
        shader_uniform(shader, "view", UNIFORM_MATRIX_4, 1, view.m);
        shader_uniform(shader, "projection", UNIFORM_MATRIX_4, 1, projection.m);

        atlas_bind(atlas);
        chunk_render(chunk);

        g_swap_buffers();
    }

    atlas_destroy(atlas);
    free_chunk(chunk);
    close_shader(shader);
    g_close();

    return 0;
}
