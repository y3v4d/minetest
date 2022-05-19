#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include <GL/glew.h>
#include <GL/glx.h>

#include "glx/vao.h"
#include "glx/vbo.h"
#include "glx/shader.h"
#include "glx/texture.h"
#include "glx/atlas.h"

#include "system.h"
#include "event.h"

#include "block.h"
#include "chunk.h"
#include "text.h"
#include "ray.h"

#include "utils/img_loader.h"
#include "utils/font_loader.h"

#include "math/vec.h"
#include "math/matrix.h"

#include "world.h"


void GLAPIENTRY
MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    if(type == GL_DEBUG_TYPE_ERROR) {
        fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity, message);
    }
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

const float C_SIZE = 2.f;
const float CURSOR_VERTICES[] = {
    -C_SIZE, C_SIZE, 0.f, 0.f,
    C_SIZE, C_SIZE, 0.f, 0.f,
    C_SIZE, -C_SIZE, 0.f, 0.f,
    -C_SIZE, -C_SIZE, 0.f, 0.f
};

const unsigned CURSOR_INDICES[] = {
    2, 1, 0,
    0, 3, 2
};

int window_width = 640;
int window_height = 480;

int main() {
    g_init();
    blocks_init();

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEPTH_TEST);
    glDebugMessageCallback(MessageCallback, 0);

    const GLubyte *version = glGetString(GL_VERSION);
    printf("OpenGL version: %s\n", version);

    world_t *world = world_init();
    if(!world) {
        fprintf(stderr, "Error creating world\n");
        return 1;
    }

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

    fontbmp_t *font = fontbmp_make("data/fonts/origami-mommy.fnt");
    if(!font) {
        fprintf(stderr, "Error loading font\n");
        return 1;
    }

    texture_t *dot_tex = texture_make("data/textures/dot.bmp");
    if(!dot_tex) {
        fprintf(stderr, "Error loading dot texture\n");
        return 1;
    }

    mat4_t model = mat4_identity();
    mat4_t view = mat4_identity();
    mat4_t projection = mat4_perspective(45.f, 640.f / 480.f, 0.1f, 10.f);

    const float SPEED = 0.1f;
    const float ROTATION_SPEED = 0.001f;

    vec3f pos = { 2.5f, 7.2f, -3.5f };//{ 0.5f, 5.5f, 0.5f };//{ 8, 9.f, -8 };
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
    int move_h = 0;

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

    vao_bind(NULL);

    text_t *text = text_make(font);
    if(!text) {
        fprintf(stderr, "Error making text\n");
        return 1;
    }

    text_set(text, "MineTest OpenGL");

    text_t *block_text = text_make(font);
    if(!block_text) {
        fprintf(stderr, "Error making block text\n");
        return 1;
    }

    text_set(block_text, "Block: Grass");

    text_t *looking = text_make(font);
    if(!looking) {
        fprintf(stderr, "Error making looking text\n");
        return 1;
    }

    text_set(looking, "Looking at");

    text_t *pos_text = text_make(font);
    if(!pos_text) {
        fprintf(stderr, "Error making pos text\n");
        return 1;
    }

    text_set(pos_text, "Pos");

    text_t *rot_text = text_make(font);
    if(!rot_text) {
        fprintf(stderr, "Error making rotation text\n");
        return 1;
    }

    text_set(rot_text, "Rot");

    mat4_t cursor_i = mat4_identity();
    mat4_t ui_projection = mat4_orthographic(0.f, 640.f, 0.f, 480.f, 0.f, 10.f);

    mat4_t text_m = mat4_identity();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    glFrontFace(GL_CW);
    glEnable(GL_CULL_FACE);

    glEnable(GL_DEPTH_CLAMP);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS); 

    glLineWidth(1.f);

    char fps_buffer[32];
    struct timespec prev_time;

    struct timespec delay_time;
    clock_gettime(CLOCK_REALTIME, &delay_time);

    raydata_t ray;
    uint8_t current_block = BLOCK_GRASS;

    bool_e lock_mouse = TRUE;

    bool_e test = TRUE;

    // event loop
    event_t event;
    short done = 0;
    while(!done) {
        while(g_pending_events()) {
            g_get_event(&event);

            dm_x = 0.0f;

            if(event.type == EVENT_KEY_PRESS) {
                char key = event.eventkey.key;

                if(key >= '0' && key <= '9') {
                    uint8_t id = key - '0';

                    if(id > BLOCK_MAX_ID) break;
                    else {
                        char s[32];
                        snprintf(s, 32, "Block: %s", block_name_from_id(id));

                        text_set(block_text, s);

                        current_block = id;
                        break;
                    }
                }

                switch(key) {
                    case 't': test = (test ? FALSE : TRUE); break;
                    case 'q': done = 1; break;
                    case 'g': lock_mouse = !lock_mouse; break;
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
                    case 'a': move_h = -1; break;
                    case 'd': move_h = 1; break;
                    case ' ':
                        vel.y = 0.2f;
                        break;
                    default: break;
                }
            } else if(event.type == EVENT_KEY_RELEASE) {
                char key = event.eventkey.key;

                if(key == 'w' || key == 's') {
                    move = 0;
                } else if(key == 'a' || key == 'd') {
                    move_h = 0;
                }
            } else if(event.type == EVENT_WINDOW_CLOSE) {
                printf("WM_DELETE_WINDOW invoked\n");
                done = 1;
            } else if(event.type == EVENT_WINDOW_RESIZE) {
                projection = mat4_perspective(60.f, (float)event.window.width / event.window.height, 0.1f, 10.f);
                ui_projection = mat4_orthographic(0.f, (float)event.window.width, 0.f, (float)event.window.height, -100.f, 100.f);

                window_width = event.window.width;
                window_height = event.window.height;
            } else if(event.type == EVENT_MOUSE_MOVE) {
                m_x = event.eventmouse.x;
                m_y = event.eventmouse.y;
            } else if(event.type == EVENT_MOUSE_PRESSED) {
                if(event.eventmouse.button == MOUSE_BUTTON_1) {
                    if(ray.valid) {
                        world_set_block(world, ray.coord.x, ray.coord.y, ray.coord.z, BLOCK_AIR);
                    }
                } else if(event.eventmouse.button == MOUSE_BUTTON_3) {
                    if(ray.valid) {
                        vec3f off = direction_to_vec3f(ray.face);

                        world_set_block(world, ray.coord.x + off.x, ray.coord.y + off.y, ray.coord.z + off.z, current_block);
                    }
                }
            }
        }

        // delta time
        struct timespec curr;
        clock_gettime(CLOCK_REALTIME, &curr);

        long curr_n = curr.tv_sec * 1000000000 + curr.tv_nsec;
        long prev_n = prev_time.tv_sec * 1000000000 + prev_time.tv_nsec;

        float delta = (float)(curr_n - prev_n) / 1000000;
        prev_time = curr;

        clock_gettime(CLOCK_REALTIME, &curr);
        curr_n = curr.tv_sec * 1000000000 + curr.tv_nsec;
        prev_n = delay_time.tv_sec * 1000000000 + delay_time.tv_nsec;

        if((curr_n - prev_n) >= 500000000) {
            snprintf(fps_buffer, 32, "FPS: %.2f", 1000.f / delta);
            text_set(text, fps_buffer);

            delay_time = curr;
        }

        // handle rotation with mouse movement
        dm_x = m_x - pm_x;
        dm_y = m_y - pm_y;

        rot.x += (-dm_y) / 5.f;
        rot.y += (-dm_x) / 5.f;

        if(rot.x > 90.f) rot.x = 90.f;
        else if(rot.x < -90.f) rot.x = -90.f;

        {
            char buff[32];

            snprintf(buff, 32, "Rot %.2f %.2f", rot.x, rot.y);
            text_set(rot_text, buff);
        }

        facing.x = -sinf(RADIANS(rot.y)) * cosf(RADIANS(rot.x));
        facing.y = sinf(RADIANS(rot.x));
        facing.z = -cosf(RADIANS(rot.y)) * cosf(RADIANS(rot.x));

        vel.x = 0;
        vel.z = 0;

        // set 3d velocity
        if(move != 0) {
            // Negative sin and cos where Y rotation
            // because the camera is looking at -z by default
            // (Y rotation responsible for the horizontal and depth movement)
            vel.x += -sinf(RADIANS(rot.y)) * move;
            //vel.y = facing.y * SPEED * move;
            vel.z += -cosf(RADIANS(rot.y)) * move;
        }

        if(move_h != 0) {
            vel.x += cosf(RADIANS(rot.y)) * move_h;
            vel.z += -sinf(RADIANS(rot.y)) * move_h;
        }

        vel.y -= 0.01f; // gravity
        if(vel.y < -0.2f) vel.y = -0.2f;

        {
            vec2f n = vec2_normalize((vec2f){vel.x, vel.z});
            vel.x = n.x * SPEED;
            vel.z = n.y * SPEED;
        }

        const float size_w = 0.3f;
        vec3f dir = {
            vel.x >= 0 ? 1 : -1,
            0,
            vel.z >= 0 ? 1 : -1
        };

        pos.x += vel.x;
        if(
            world_get_block(world, pos.x + size_w * dir.x, pos.y - 1.5f, ceilf(pos.z + size_w)) || 
            world_get_block(world, pos.x + size_w * dir.x, pos.y - 1.5f, ceilf(pos.z - size_w)) ||
            world_get_block(world, pos.x + size_w * dir.x, pos.y - 1.0f, ceilf(pos.z + size_w)) || 
            world_get_block(world, pos.x + size_w * dir.x, pos.y - 1.0f, ceilf(pos.z - size_w)) ||
            world_get_block(world, pos.x + size_w * dir.x, pos.y, ceilf(pos.z + size_w)) || 
            world_get_block(world, pos.x + size_w * dir.x, pos.y, ceilf(pos.z - size_w))
        ) {
            pos.x -= vel.x;
        }

        pos.z += vel.z;
        if(
            world_get_block(world, pos.x + size_w, pos.y - 1.5f, ceilf(pos.z + size_w * dir.z)) || 
            world_get_block(world, pos.x - size_w, pos.y - 1.5f, ceilf(pos.z + size_w * dir.z)) ||
            world_get_block(world, pos.x + size_w, pos.y - 1.0f, ceilf(pos.z + size_w * dir.z)) || 
            world_get_block(world, pos.x - size_w, pos.y - 1.0f, ceilf(pos.z + size_w * dir.z)) ||
            world_get_block(world, pos.x + size_w, pos.y, ceilf(pos.z + size_w * dir.z)) || 
            world_get_block(world, pos.x - size_w, pos.y, ceilf(pos.z + size_w * dir.z))
        ) {
            pos.z -= vel.z;
        }

        pos.y += vel.y;
        { // y check
            vec3i check = {
                .y = pos.y - 1.5f
            };
            bool_e fall = TRUE;

            if(world_get_block(world, floorf(pos.x + size_w), check.y, ceilf(pos.z + size_w))) {
                fall = FALSE;
            } else if(world_get_block(world, floorf(pos.x - size_w), check.y, ceilf(pos.z + size_w))) {
                fall = FALSE;
            } else if(world_get_block(world, floorf(pos.x + size_w), check.y, ceilf(pos.z - size_w))) {
                fall = FALSE;
            } else if(world_get_block(world, floorf(pos.x - size_w), check.y, ceilf(pos.z - size_w))) {
                fall = FALSE;
            }

            if(!fall) {
                pos.y = floorf(pos.y) + 0.5f;
                vel.y = 0;
            }

            check.y = pos.y + 0.3f;

            if(world_get_block(world, floorf(pos.x + size_w), check.y, ceilf(pos.z + size_w))) {
                pos.y -= vel.y;
                vel.y = 0;
            } else if(world_get_block(world, floorf(pos.x - size_w), check.y, ceilf(pos.z + size_w))) {
                pos.y -= vel.y;
                vel.y = 0;
            } else if(world_get_block(world, floorf(pos.x + size_w), check.y, ceilf(pos.z - size_w))) {
                pos.y -= vel.y;
                vel.y = 0;
            } else if(world_get_block(world, floorf(pos.x - size_w), check.y, ceilf(pos.z - size_w))) {
                pos.y -= vel.y;
                vel.y = 0;
            }
        }

        {
            char buff[32];

            snprintf(buff, 32, "Pos %.2f %.2f %.2f", pos.x, pos.y, pos.z);
            text_set(pos_text, buff);
        }

        const vec3f shifted_pos = (vec3f){
            pos.x,
            pos.y,
            pos.z
        };

        get_block_with_ray(world, &shifted_pos, &facing, &ray);

        if(ray.valid) {
            char buff[32];

            snprintf(buff, 32, "Looking at %d %d %d", ray.coord.x, ray.coord.y, ray.coord.z);
            text_set(looking, buff);
        } else {
            text_set(looking, "Looking at - - -");
        }

        if(lock_mouse) {
            g_lock_mouse();
            pm_x = 320; pm_y = 240;
            m_x = 320; m_y = 240;
        }
        
        view = mat4_identity();
        view = mat4_mul_mat4(mat4_translation(-pos.x, -pos.y, -pos.z), view);

        view = mat4_mul_mat4(mat4_rotation_y(-rot.y), view);
        view = mat4_mul_mat4(mat4_rotation_x(-rot.x), view);

        if(test) view = mat4_mul_mat4(mat4_translation(0.f, 0.f, 0.6f), view);
        
        glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glDepthFunc(GL_LESS); 

        model.m[3] = 0.f;
        model.m[7] = 0.f;
        model.m[11] = 0.f;

        shader_use(shader);
        shader_uniform(shader, "model", UNIFORM_MATRIX_4, 1, model.m);
        shader_uniform(shader, "view", UNIFORM_MATRIX_4, 1, view.m);
        shader_uniform(shader, "projection", UNIFORM_MATRIX_4, 1, projection.m);

        atlas_bind(atlas);
        world_render(world, shader);

        if(ray.valid) {
            model.m[3] = (int)ray.coord.x;
            model.m[7] = (int)ray.coord.y;
            model.m[11] = (int)ray.coord.z;

            shader_uniform(shader, "model", UNIFORM_MATRIX_4, 1, model.m);

            vao_bind(&highlight_vao);
            vbo_bind(&highlight_vbo);
            vbo_bind(&highlight_vio);
            glDrawElements(GL_LINES, sizeof(HIGH_INDICES), GL_UNSIGNED_INT, (void*)0);
        }

        // render UI
        glDepthFunc(GL_ALWAYS); 

        shader_use(text_shader);
        shader_uniform(text_shader, "projection", UNIFORM_MATRIX_4, 1, ui_projection.m);

        text_m.m[3] = window_width / 2.f;
        text_m.m[7] = window_height / 2.f;
        text_m.m[11] = 0.f;
        shader_uniform(text_shader, "model", UNIFORM_MATRIX_4, 1, text_m.m);

        glActiveTexture(GL_TEXTURE0);
        texture_bind(dot_tex);

        vao_bind(&cursor_vao);
        vbo_bind(&cursor_vbo);
        vbo_bind(&cursor_vio);
        glDrawElements(GL_TRIANGLES, sizeof(CURSOR_INDICES), GL_UNSIGNED_INT, (void*)0);

        text_m.m[3] = 0.f;
        text_m.m[7] = 0.f;
        shader_uniform(text_shader, "model", UNIFORM_MATRIX_4, 1, text_m.m);
        text_render(text);

        text_m.m[3] = 0.f;
        text_m.m[7] = 32.f;
        shader_uniform(text_shader, "model", UNIFORM_MATRIX_4, 1, text_m.m);
        text_render(block_text);

        text_m.m[3] = 0.f;
        text_m.m[7] = window_height - 32.f;
        shader_uniform(text_shader, "model", UNIFORM_MATRIX_4, 1, text_m.m);
        text_render(rot_text);

        text_m.m[3] = 0.f;
        text_m.m[7] = window_height - 64.f;
        shader_uniform(text_shader, "model", UNIFORM_MATRIX_4, 1, text_m.m);
        text_render(pos_text);

        g_swap_buffers();
    }

    texture_destroy(dot_tex);

    text_destroy(pos_text);
    text_destroy(looking);
    text_destroy(block_text);
    text_destroy(text);
    text_destroy(rot_text);
    fontbmp_close(font);

    vbo_destroy(&highlight_vbo);
    vbo_destroy(&highlight_vio);
    vao_destroy(&highlight_vao);

    vbo_destroy(&cursor_vbo);
    vbo_destroy(&cursor_vio);
    vao_destroy(&cursor_vao);

    atlas_destroy(atlas);
    world_destroy(world);
    close_shader(text_shader);
    close_shader(shader);
    g_close();

    return 0;
}
