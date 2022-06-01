#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>


#include <GL/glew.h>
#include <GL/glx.h>

#include "glx/vao.h"
#include "glx/vbo.h"
#include "glx/shader.h"
#include "glx/texture.h"
#include "glx/atlas.h"

#include "core/system.h"
#include "core/event.h"

#include "block.h"
#include "chunk.h"
#include "highlight.h"
#include "text.h"
#include "ray.h"

#include "utils/img_loader.h"
#include "utils/font_loader.h"

#include "math/vec.h"
#include "math/matrix.h"

#include "direction.h"
#include "camera.h"

#include "sprite.h"

#include "world.h"

#include "stoper.h"

const float FOV = 60.f;
int window_width = 1280;
int window_height = 720;

int main() {
    g_init();
    blocks_init();

    world_t *world = world_init();
    if(!world) return 1;

    shader_t *shader = make_shader("data/shaders/main");
    if(!shader) return 1;

    shader_t *ui_shader = make_shader("data/shaders/ui");
    if(!ui_shader) return 1;

    atlas_t *atlas = atlas_generate("data/textures/atlas.bmp", 16, 16);
    if(!atlas) return 1;

    fontbmp_t *font = fontbmp_make("data/fonts/origami-mommy.fnt");
    if(!font) return 1;

    texture_t *dot_tex = texture_make("data/textures/dot.bmp");
    if(!dot_tex) return 1;

    camera_t *camera = camera_init(CAMERA_PROJECTION_PERSPECTIVE, (float)window_width / window_height, FOV);
    if(!camera) return 1;

    highlight_t *highlight = highlight_create();
    if(!highlight) return 1;

    sprite_t *s_cursor = sprite_init(F2VEC3F((float)window_width / 2, (float)window_height / 2, 0.f));
    if(!s_cursor) return 1;

    s_cursor->texture = dot_tex;

    text_t *title = text_make(font, "MineTest OpenGL", (vec3f) { 0.f, 0.f, 0.f });
    if(!title) return 1;

    text_t *block_text = text_make(font, "Block: Grass", (vec3f) { 0.f, 32.f, 0.f });
    if(!block_text) return 1;

    text_t *mode = text_make(font, "Mode: Default", (vec3f) { 0.f, 64.f, 0.f });
    if(!mode) return 1;

    text_t *camera_mode = text_make(font, "Camera Mode: FILL", (vec3f) { 0.f, 96.f, 0.f });
    if(!camera_mode) return 1;

    text_t *looking = text_make(font, "Looking at", (vec3f) { 0.f, window_height - 32.f, 0.f });
    if(!looking) return 1;

    text_t *pos_text = text_make(font, "Pos", (vec3f) { 0.f, window_height - 96.f, 0.f });
    if(!pos_text) return 1;

    text_t *rot_text = text_make(font, "Rot", (vec3f) { 0.f, window_height - 64.f, 0.f });
    if(!rot_text) return 1;

    camera_t *ui_camera = camera_init(CAMERA_PROJECTION_ORTHOGRAPHIC, 0.f, 0.f);
    if(!ui_camera) return 1;

    const float SPEED = 0.1f;
    const float ROTATION_SPEED = 0.001f;

    vec3f vel = { 0.f, 0.f, 0.f };

    float m_x = 0.0f;
    float m_y = 0.0f;

    float pm_x = -1.0f;
    float pm_y = -1.0f;

    float dm_x = 0.0f;
    float dm_y = 0.0f;

    int move = 0;
    int move_h = 0;

    const GLubyte *version = glGetString(GL_VERSION);
    printf("OpenGL version: %s\n", version);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    glFrontFace(GL_CW);
    glEnable(GL_CULL_FACE);

    glEnable(GL_DEPTH_CLAMP);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS); 

    glLineWidth(1.f);

    stoper_t dt_stoper;

    raydata_t ray;
    uint8_t current_block = BLOCK_GRASS;

    bool_e lock_mouse = TRUE;

    bool_e test = TRUE;
    bool_e free_cam = FALSE;

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
                    case 'v':
                        camera->mode = (camera->mode == CAMERA_MODE_LINE ? CAMERA_MODE_FILL : CAMERA_MODE_LINE);
                        text_set(camera_mode, (camera->mode == CAMERA_MODE_LINE ? "Camera Mode: LINE" : "Camera Mode: FILL"));
                        break;
                    case 'f': 
                        free_cam = !free_cam;
                        text_set(mode, (free_cam ? "Mode: FREE CAM" : "Mode: DEFAULT"));

                        break;
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
                        //if(!free_cam) vel.y = 0.2f;
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
                camera->aspect = (float)event.window.width / event.window.height;
                camera_update(camera);

                ui_camera->metrics = (camera_metrics_t) {
                    .left = 0.f,
                    .right = (float)event.window.width,
                    .top = 0.f,
                    .bottom = (float)event.window.height
                };
                ui_camera->near = 0.f;
                ui_camera->far = 100.f;
                camera_update(ui_camera);

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
                        vec3i off = DIR2VEC3I(ray.face);
                        world_set_block(world, ray.coord.x + off.x, ray.coord.y + off.y, ray.coord.z - off.z, current_block);
                    }
                }
            }
        }

        // delta time
        stoper_end(&dt_stoper);
        stoper_start(&dt_stoper);

        // handle rotation with mouse movement
        dm_x = m_x - pm_x;
        dm_y = m_y - pm_y;

        camera->rotation.x += (-dm_y) / 5.f;
        camera->rotation.y += (-dm_x) / 5.f;
        camera_update(camera);

        vel.x = 0;
        if(free_cam) vel.y = 0;
        vel.z = 0;

        // set 3d velocity
        if(move != 0) {
            // Negative sin and cos where Y rotation
            // because the camera is looking at -z by default
            // (Y rotation responsible for the horizontal and depth movement)
            vel.x += -sinf(RADIANS(camera->rotation.y)) * move;
            if(free_cam) vel.y = camera->facing.y * SPEED * move;
            vel.z += -cosf(RADIANS(camera->rotation.y)) * move;
        }

        if(move_h != 0) {
            vel.x += cosf(RADIANS(camera->rotation.y)) * move_h;
            vel.z += -sinf(RADIANS(camera->rotation.y)) * move_h;
        }

        //vel.y -= (free_cam ? 0 : 0.01f); // gravity
        //if(vel.y < -0.2f) vel.y = -0.2f;

        {
            vec2f n = vec2f_normalize(F2VEC2F(vel.x, vel.z));
            vel.x = n.x * SPEED;
            vel.z = n.y * SPEED;
        }

        vec3f_add(&camera->position, vel);

        camera_update(camera);
        get_block_with_ray(world, &camera->position, &camera->facing, &ray);

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

        {
            char buff[32];

            snprintf(buff, 32, "Pos %.2f %.2f %.2f", camera->position.x, camera->position.y, camera->position.z);
            text_set(pos_text, buff);

            snprintf(buff, 32, "Rot %.2f %.2f", camera->rotation.x, camera->rotation.y);
            text_set(rot_text, buff);

            snprintf(buff, 32, "FPS: %.2f", 1000.f / dt_stoper.delta);
            text_set(title, buff);

            if(ray.valid) {
                snprintf(buff, 32, "Looking at %d %d %d", ray.coord.x, ray.coord.y, ray.coord.z);
                text_set(looking, buff);
            } else {
                text_set(looking, "Looking at - - -");
            }
        }
        
        glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glDepthFunc(GL_LESS); 

        shader_use(shader);
        camera_use(camera, shader);

        atlas_bind(atlas);
        world_render(world, shader);

        if(ray.valid) {
            highlight->position = VEC3I2F(ray.coord);
            highlight_render(highlight, shader);
        }

        // render UI
        glDepthFunc(GL_ALWAYS); 

        shader_use(ui_shader);
        camera_use(ui_camera, ui_shader);

        sprite_render(s_cursor, ui_shader);

        text_render(title, ui_shader);
        text_render(camera_mode, ui_shader);
        text_render(block_text, ui_shader);
        text_render(mode, ui_shader);
        text_render(looking, ui_shader);
        text_render(rot_text, ui_shader);
        text_render(pos_text, ui_shader);

        g_swap_buffers();
    }

    texture_destroy(dot_tex);

    text_destroy(pos_text);
    text_destroy(looking);
    text_destroy(block_text);
    text_destroy(title);
    text_destroy(rot_text);
    text_destroy(mode);
    text_destroy(camera_mode);
    fontbmp_close(font);

    sprite_destroy(s_cursor);

    highlight_destroy(highlight);

    camera_destroy(ui_camera);
    camera_destroy(camera);

    atlas_destroy(atlas);
    world_destroy(world);
    close_shader(ui_shader);
    close_shader(shader);
    g_close();

    return 0;
}
