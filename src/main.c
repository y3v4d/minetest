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

    // load texture atlas
    bmp_t *img = load_bmp("data/textures/atlas.bmp");
    if(!img) {
        fprintf(stderr, "Couldn't load texture!\n");
        return 1;
    }

    unsigned int texture;
    const unsigned TILE_W = 16, TILE_H = 16;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGB8, TILE_W, TILE_H, 4);

    byte_t tile_buffer[TILE_W * TILE_H * 3];
    byte_t *start = img->data;
    for(int y = 0; y < img->height / TILE_H; ++y) {
        for(int x = 0; x < img->width / TILE_W; ++x) {
            start = img->data + ((y * TILE_H * img->width) + TILE_W * x) * 3;

            for(int row = 0; row < TILE_H; ++row) {
                memcpy(tile_buffer + row * TILE_W * 3, start + row * img->width * 3, TILE_W * 3);
            }

            glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, (y * 2 + x), TILE_W, TILE_H, 1, GL_BGR, GL_UNSIGNED_BYTE, tile_buffer);
        }
    }

    free_bmp(img);

    mat4_t model = mat4_identity();
    mat4_t view = mat4_identity();
    mat4_t projection = mat4_perspective(60.f, 640.f / 480.f, 0.1f, 10.f);

    GLint model_location = glGetUniformLocation(shader->program, "model");
    GLint view_location = glGetUniformLocation(shader->program, "view");
    GLint projection_location = glGetUniformLocation(shader->program, "projection");

    const float SPEED = 0.1f;
    const float ROTATION_SPEED = 0.001f;
    float v_x = 0.0f;
    float v_y = 0.0f;
    float v_z = 0.0f;

    float angle_y = 0.f;
    float angle_z = 0.f;
    float v_ay = 0.0f;
    float v_az = 0.0f;

    float m_x = 0.0f;
    float m_y = 0.0f;

    float pm_x = -1.0f;
    float pm_y = -1.0f;

    float dm_x = 0.0f;
    float dm_y = 0.0f;

    vec3f cam_pos = { 0.f, 0.f, 0.f };

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
                        v_z = cosf(RADIANS(angle_y)) * cosf(RADIANS(angle_z)) * -SPEED;
                        v_x = sinf(RADIANS(-angle_y)) * cosf(RADIANS(angle_z)) * -SPEED;
                        v_y = sinf(RADIANS(-angle_z)) * -SPEED;
                        break;
                    case 's':
                        v_z = cosf(RADIANS(angle_y)) * cosf(RADIANS(angle_z)) * SPEED;
                        v_x = sinf(RADIANS(-angle_y)) * cosf(RADIANS(angle_z)) * SPEED;
                        v_y = sinf(RADIANS(-angle_z)) * SPEED;
                        break;
                    default: break;
                }
            } else if(event.type == EVENT_KEY_RELEASE) {
                char key = event.eventkey.key;

                if(key == 'w' || key == 's') {
                    v_x = 0;
                    v_y = 0;
                    v_z = 0;
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

        angle_y += (dm_x) / 10000.0f;
        angle_z += (-dm_y) / 10000.0f;

        cam_pos.x += v_x;
        cam_pos.y += v_y;
        cam_pos.z += v_z;

        g_lock_mouse();
        pm_x = 320; pm_y = 240;
        m_x = 320; m_y = 240;

        view = mat4_rotation_x(-angle_z);
        view = mat4_mul_mat4(view, mat4_rotation_y(angle_y));
        view = mat4_mul_mat4(view, mat4_translation(-cam_pos.x, -cam_pos.y, -cam_pos.z));

        glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader->program);
        glUniformMatrix4fv(model_location, 1, GL_TRUE, model.m);
        glUniformMatrix4fv(view_location, 1, GL_TRUE, view.m);
        glUniformMatrix4fv(projection_location, 1, GL_TRUE, projection.m);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
        chunk_render(chunk);

        g_swap_buffers();
    }

    free_chunk(chunk);
    close_shader(shader);
    g_close();

    return 0;
}
