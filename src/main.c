#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#include <GL/glew.h>
#include <GL/glx.h>

#include "glx/vao.h"
#include "glx/vbo.h"
#include "system.h"
#include "shader.h"
#include "event.h"

#include "utils/img_loader.h"
#include "utils/font_loader.h"

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

float absf(float i) {
    return (i < 0.f ? i * -1 : i);
}

const float HIGH_VERTICES[] = {
    0.f,    1.f,    0.f,    4.f,    // 0 - left top front
    1.f,    1.f,    0.f,    4.f,    // 1 - right top front
    1.f,    0.f,    0.f,    4.f,    // 2 - right bottom front
    0.f,    0.f,    0.f,    4.f,    // 3 - left bottom front

    1.f,    1.f,    -1.f,    4.f,   // 4 - right top back
    0.f,    1.f,    -1.f,    4.f,   // 5 - left top back
    0.f,    0.f,    -1.f,    4.f,   // 6 - left bottom back
    1.f,    0.f,    -1.f,    4.f,   // 7 - right bottom back
};

const unsigned HIGH_INDICES[] = {
    0, 1, 1, 4, 4, 5, 5, 0,
    3, 2, 2, 7, 7, 6, 6, 3,
    0, 3, 1, 2, 4, 7, 5, 6
};

const float C_SIZE = 0.005f;
const float CURSOR_VERTICES[] = {
    -C_SIZE, C_SIZE, 0.f, 4.f,
    C_SIZE, C_SIZE, 0.f, 4.f,
    C_SIZE, -C_SIZE, 0.f, 4.f,
    -C_SIZE, -C_SIZE, 0.f, 4.f
};

const unsigned CURSOR_INDICES[] = {
    0, 1, 2,
    2, 3, 0
};

const float CHAR_VERTICES[] = {
    0.f, 0.f, 0.f, 114.f / 256.f, 1.f - (85.f / 256.f),
    14.f, 0.f, 0.f, (114.f + 17.f) / 256.f, 1.f - (85.f / 256.f),
    14.f, 22.f, 0.f, (114.f + 17.f) / 256.f, 1.f - (107.f / 256.f),
    0.f, 22.f, 0.f, 114.f / 256.f, 1.f - (107.f / 256.f)
};

const unsigned CHAR_INDICES[] = {
    0, 1, 2,
    2, 3, 0
};

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

    shader_t *text_shader = make_shader("data/shaders/text");
    if(!shader) {
        fprintf(stderr, "Error making shader text\n");
        return 1;
    }

    atlas_t *atlas = atlas_generate("data/textures/atlas.bmp", 16, 16);
    if(!atlas) {
        return 1;
    }

    bmp_t *font_bmp = load_bmp("data/fonts/solway_0.bmp", 1, 0);
    if(!font_bmp) {
        fprintf(stderr, "Couldn't locate the font\n");
        return 1;
    }

    mat4_t model = mat4_identity();
    mat4_t view = mat4_identity();
    mat4_t projection = mat4_perspective(45.f, 640.f / 480.f, 0.1f, 10.f);

    const float SPEED = 0.1f;
    const float ROTATION_SPEED = 0.001f;

    vec3f pos = { 8, 19, -8 };
    vec3f vel = { 0.f, 0.f, 0.f };

    vec2f rot = { 0.f, 0.f };

    vec3f facing = { 0.f, 0.f, 0.f };

    float m_x = 0.0f;
    float m_y = 0.0f;

    float pm_x = -1.0f;
    float pm_y = -1.0f;

    float dm_x = 0.0f;
    float dm_y = 0.0f;

    int move = 0;
    int found = 0;

    vec3f check;
    direction_e check_face;

    vbo_t highlight_vio = vbo_generate(GL_ELEMENT_ARRAY_BUFFER, FALSE);
    vbo_t highlight_vbo = vbo_generate(GL_ARRAY_BUFFER, FALSE);
    vao_t highlight_vao = vao_generate();

    vao_bind(&highlight_vao);
    vbo_bind(&highlight_vbo);
    vbo_data(&highlight_vbo, sizeof(HIGH_VERTICES), HIGH_VERTICES);

    vbo_bind(&highlight_vio);
    vbo_data(&highlight_vio, sizeof(HIGH_INDICES), HIGH_INDICES);

    vao_attribute(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    vao_attribute(2, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(3 * sizeof(float)));

    vao_bind(NULL);

    vbo_t cursor_vio = vbo_generate(GL_ELEMENT_ARRAY_BUFFER, FALSE);
    vbo_t cursor_vbo = vbo_generate(GL_ARRAY_BUFFER, FALSE);
    vao_t cursor_vao = vao_generate();

    vao_bind(&cursor_vao);
    vbo_bind(&cursor_vbo);
    vbo_data(&cursor_vbo, sizeof(CURSOR_VERTICES), CURSOR_VERTICES);

    vbo_bind(&cursor_vio);
    vbo_data(&cursor_vio, sizeof(CURSOR_INDICES), CURSOR_INDICES);

    vao_attribute(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    vao_attribute(2, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(3 * sizeof(float)));

    vbo_t char_vio = vbo_generate(GL_ELEMENT_ARRAY_BUFFER, FALSE);
    vbo_t char_vbo = vbo_generate(GL_ARRAY_BUFFER, FALSE);
    vao_t char_vao = vao_generate();

    vao_bind(&char_vao);
    vbo_bind(&char_vbo);
    vbo_data(&char_vbo, sizeof(CHAR_VERTICES), CHAR_VERTICES);

    vbo_bind(&char_vio);
    vbo_data(&char_vio, sizeof(CHAR_INDICES), CHAR_INDICES);

    vao_attribute(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    vao_attribute(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    vao_bind(NULL);

    unsigned char_id = 0;
    glGenTextures(1, &char_id);
    glBindTexture(GL_TEXTURE_2D, char_id);

    glPixelStoref(GL_UNPACK_ALIGNMENT, 1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, font_bmp->width, font_bmp->height, 0, GL_BGRA, GL_UNSIGNED_BYTE, font_bmp->data);

    free_bmp(font_bmp);

    vec3i tile;

    mat4_t cursor_i = mat4_identity();
    mat4_t cursor_p = mat4_orthographic(0.f, 640.f, 0.f, 640.f, 0.f, 10.f);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    shader_use(shader);

    // event loop
    event_t event;
    short done = 0;
    glLineWidth(1.f);
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
            } else if(event.type == EVENT_MOUSE_PRESSED) {
                if(event.eventmouse.button == MOUSE_BUTTON_1) {
                    if(found) {
                        set_chunk_block(chunk, tile.x, tile.y, tile.z, 0);
                        found = 0;
                        prepare_chunk(chunk);
                    }
                } else if(event.eventmouse.button == MOUSE_BUTTON_3) {
                    if(found) {
                        vec3f off = direction_to_vec3f(check_face);
                        set_chunk_block(chunk, tile.x + off.x, tile.y + off.y, tile.z + off.z, 1);
                        found = 0;
                        prepare_chunk(chunk);
                    }
                }
            }
        }

        dm_x = m_x - pm_x;
        dm_y = m_y - pm_y;

        rot.x += (-dm_y) / 5.f;
        rot.y += (-dm_x) / 5.f;

        if(rot.x > 90.f) rot.x = 90.f;
        else if(rot.x < -90.f) rot.x = -90.f;

        facing.x = -sinf(RADIANS(rot.y)) * cosf(RADIANS(rot.x));
        facing.y = sinf(RADIANS(rot.x));
        facing.z = -cosf(RADIANS(rot.y)) * cosf(RADIANS(rot.x));

        //printf("Pos (%f, %f, %f)\n", pos.x, pos.y, pos.z);
        //printf("Facing (%f, %f, %f)\n", facing.x, facing.y, facing.z);

        vec3f direction = {
            (facing.x >= 0 ? 1 : -1),
            (facing.y >= 0 ? 1 : -1),
            (facing.z >= 0 ? 1 : -1)
        };

        tile = (vec3i) {
            floorf(pos.x),
            floorf(pos.y),
            ceilf(pos.z)
        };

        float offsetX = (direction.x >= 0 ? 1 : 0);
        float offsetY = (direction.y >= 0 ? 1 : 0);
        float offsetZ = (direction.z >= 0 ? 0 : -1);

        float t = 0;

        check = pos;
        for(int i = 0; i < 16; ++i) {
            vec3f distance = {
                tile.x - check.x + offsetX,
                tile.y - check.y + offsetY,
                tile.z - check.z + offsetZ
            };

            distance.x /= facing.x;
            distance.y /= facing.y;
            distance.z /= facing.z;
            
            if(distance.x < distance.z) {
                if(distance.y < distance.x) {
                    if(facing.y < 0) {
                        tile.y += -1;
                        check_face = TOP;
                    } else {
                        tile.y += 1;
                        check_face = BOTTOM;
                    }

                    t += distance.y;
                } else {
                    if(facing.x < 0) {
                        tile.x += -1;
                        check_face = RIGHT;
                    } else {
                        tile.x += 1;
                        check_face = LEFT;
                    }

                    t += distance.x;
                }
            } else {
                if(distance.y < distance.z) {
                    if(facing.y < 0) {
                        tile.y += -1;
                        check_face = TOP;
                    } else {
                        tile.y += 1;
                        check_face = BOTTOM;
                    }

                    t += distance.y;
                } else {
                    if(facing.z < 0) {
                        check_face = FRONT;
                        tile.z += -1;
                    } else {
                        check_face = BACK;
                        tile.z += 1;
                    }

                    t += distance.z;
                }
            }

            check.x = pos.x + facing.x * t;
            check.y = pos.y + facing.y * t;
            check.z = pos.z + facing.z * t;

            if(get_chunk_block(chunk, tile.x, tile.y, tile.z)) {
                found = 1;
                //printf("Looking at (%d, %d, %d) Face: %s\n", tile.x, tile.y, tile.z, direction_name(check_face));
                break;
            } else found = 0;
        }

        if(move != 0) {
            // Negative sin and cos where Y rotation
            // because the camera is looking at -z by default
            // (Y rotation responsible for the horizontal and depth movement)
            vel.x = facing.x * SPEED * move;
            vel.y = facing.y * SPEED * move;
            vel.z = facing.z * SPEED * move;
        } else {
            vel.x = 0;
            vel.y = 0;
            vel.z = 0;
        }

        //vel.y -= 0.01f; // gravity
        //if(vel.y < -0.1f) vel.y = -0.2f;

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

        /*model.m[3] = 0.f;
        model.m[7] = 0.f;
        model.m[11] = 0.f;

        shader_uniform(shader, "model", UNIFORM_MATRIX_4, 1, cursor_i.m);
        shader_uniform(shader, "view", UNIFORM_MATRIX_4, 1, cursor_i.m);

        vao_bind(&cursor_vao);
        vbo_bind(&cursor_vbo);
        vbo_bind(&cursor_vio);
        glDrawElements(GL_TRIANGLES, sizeof(CURSOR_INDICES), GL_UNSIGNED_INT, (void*)0);
        
        shader_uniform(shader, "model", UNIFORM_MATRIX_4, 1, model.m);
        shader_uniform(shader, "view", UNIFORM_MATRIX_4, 1, view.m);
        shader_uniform(shader, "projection", UNIFORM_MATRIX_4, 1, projection.m);

        atlas_bind(atlas);
        chunk_render(chunk);

        if(found) {
            model.m[3] = (int)tile.x;
            model.m[7] = (int)tile.y;
            model.m[11] = (int)tile.z;

            shader_uniform(shader, "model", UNIFORM_MATRIX_4, 1, model.m);

            vao_bind(&highlight_vao);
            vbo_bind(&highlight_vbo);
            vbo_bind(&highlight_vio);
            glDrawElements(GL_LINES, sizeof(HIGH_INDICES), GL_UNSIGNED_INT, (void*)0);
        }*/

        shader_use(text_shader);
        shader_uniform(text_shader, "view", UNIFORM_MATRIX_4, 1, cursor_i.m);
        shader_uniform(text_shader, "projection", UNIFORM_MATRIX_4, 1, cursor_p.m);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, char_id);

        vao_bind(&char_vao);
        vbo_bind(&char_vbo);
        vbo_bind(&char_vio);
        glDrawElements(GL_TRIANGLES, sizeof(CHAR_INDICES), GL_UNSIGNED_INT, (void*)0);

        g_swap_buffers();
    }

    vbo_destroy(&cursor_vbo);
    vbo_destroy(&cursor_vio);
    vao_destroy(&cursor_vao);

    vbo_destroy(&highlight_vbo);
    vbo_destroy(&highlight_vio);
    vao_destroy(&highlight_vao);

    vbo_destroy(&cursor_vbo);
    vbo_destroy(&cursor_vio);
    vao_destroy(&cursor_vao);

    atlas_destroy(atlas);
    free_chunk(chunk);
    close_shader(text_shader);
    close_shader(shader);
    g_close();

    return 0;
}
