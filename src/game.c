#include "game.h"

#include "ray.h"
#include "sys/event.h"
#include "utils/stoper.h"
#include "utils/types.h"
#include "world.h"
#include "block.h"
#include "camera.h"
#include "chunk.h"
#include "glx/atlas.h"
#include "glx/shader.h"
#include "highlight.h"
#include "sprite.h"
#include "sys/system.h"
#include "text.h"
#include "utils/font_loader.h"
#include "debugmenu.h"

#include "state.h"

#include <stdio.h>

state_t state;

// assets
camera_t *g_cam_ui = NULL;

fontbmp_t *g_font = NULL;

texture_t *g_tex_dot = NULL;
sprite_t  *g_spr_cursor = NULL;

debugmenu_t *debugmenu = NULL;

bool_e g_mouse_locked = TRUE;
bool_e g_free_cam = FALSE;

event_t g_event;
bool_e g_done = FALSE;

vec2f g_mouse_pos;
vec2f g_mouse_prev_pos;

stoper_t stoper_dt;

int game_init() {
    state.window_width = 1280;
    state.window_height = 720;

    g_init(state.window_width, state.window_height);
    blocks_init();

    // load assets
    if(!(state.world = world_init())) goto init_error;

    if(!(g_cam_ui = camera_init(CAMERA_PROJECTION_ORTHOGRAPHIC, 0.f, 0.f))) goto init_error;    
    
    if(!(state.atlas = atlas_generate("data/textures/atlas.bmp", 16, 16))) goto init_error;
    if(!(g_font = fontbmp_make("data/fonts/origami-mommy.fnt"))) goto init_error;
    
    if(!(g_tex_dot = texture_make("data/textures/dot.bmp"))) goto init_error; 
    if(!(g_spr_cursor = sprite_init(F2VEC3F((float)state.window_width / 2, (float)state.window_height / 2, 0.f)))) goto init_error;
    
    if(!(state.shader_main = shader_init("data/shaders/main"))) goto init_error;
    if(!(state.shader_ui = shader_init("data/shaders/ui"))) goto init_error;

    if(!(debugmenu = debugmenu_init(g_font))) goto init_error;

    // setup opengl
    // blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    // culling
    glFrontFace(GL_CW);
    glEnable(GL_CULL_FACE);

    // depth testing & clamping
    glEnable(GL_DEPTH_CLAMP);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS); 

    // for highlight
    glLineWidth(1.f);

    return 0;

    init_error:
        game_close();
        return -1;
}

void game_close() {
    // release assets
    if(debugmenu) debugmenu_destroy(debugmenu);

    if(state.shader_ui) shader_destroy(state.shader_ui);
    if(state.shader_main) shader_destroy(state.shader_main);

    if(state.atlas) atlas_destroy(state.atlas);

    if(g_spr_cursor) sprite_destroy(g_spr_cursor);
    if(g_tex_dot) texture_destroy(g_tex_dot);

    if(g_font) fontbmp_close(g_font);

    if(g_cam_ui) camera_destroy(g_cam_ui);

    if(state.world) world_destroy(state.world);

    g_close();
}

void game_on_key_press(eventkey_t *event) {
    player_t *p = state.world->player;
    char key = event->key;

    if(key >= '0' && key <= '9') {
        uint8_t id = key - '0';

        if(id > BLOCK_MAX_ID) return;
        else {
            p->current_block = id;
            return;
        }
    }

    switch(key) {
        case 'v':
            p->camera->mode = (p->camera->mode == CAMERA_MODE_LINE ? CAMERA_MODE_FILL : CAMERA_MODE_LINE);
            break;
        case 'f': 
            g_free_cam = !g_free_cam;
            break;
        case 'q': g_done = 1; break;
        case 'g': g_mouse_locked = !g_mouse_locked; break;
        case 'r':
            shader_destroy(state.shader_main);
            state.shader_main = shader_init("data/shaders/main");
            break;
        case 'w':
            p->moving.forward = 0.1f;
            break;
        case 's':
            p->moving.forward = -0.1f;
            break;
        case 'a': p->moving.right = -0.1f; break;
        case 'd': p->moving.right = 0.1f; break;
        case ' ':
            if(!g_free_cam) p->moving.up = 0.2f;
            break;
        default: break;
    }
}

void game_on_key_release(eventkey_t *event) {
    char key = event->key;

    if(key == 'w' || key == 's') {
        state.world->player->moving.forward = 0;
    } else if(key == 'a' || key == 'd') {
        state.world->player->moving.right = 0;
    }
}

void game_on_window_resize(eventwindow_t *event) {
    camera_t *c_main = state.world->player->camera;

    c_main->aspect = (float)event->width / event->height;
    camera_update(c_main);

    g_cam_ui->metrics = (camera_metrics_t) {
        .left = 0.f,
        .right = (float)event->width,
        .top = 0.f,
        .bottom = (float)event->height
    };

    g_cam_ui->near = 0.f;
    g_cam_ui->far = 100.f;
    camera_update(g_cam_ui);

    state.window_width = event->width;
    state.window_height = event->height;
}

void game_on_mouse_move(event_mouse_t *event) {
    g_mouse_pos = F2VEC2F(event->x, event->y);
}

void game_on_mouse_pressed(event_mouse_t *event) {
    raydata_t *ray = &state.world->player->ray;
    if(event->button == MOUSE_BUTTON_1) {
        if(ray->valid) {
            world_set_block(
                state.world, 
                ray->coord.x, 
                ray->coord.y, 
                ray->coord.z, 
                BLOCK_AIR
            );
        }
    } else if(event->button == MOUSE_BUTTON_3) {
        if(ray->valid) {
            vec3i off = DIR2VEC3I(ray->face);

            world_set_block(
                state.world, 
                ray->coord.x + off.x,
                ray->coord.y + off.y, 
                ray->coord.z - off.z, 
                state.world->player->current_block
            );
        }
    }
}

void game_loop() {
    while(!g_done) {
        while(g_pending_events()) {
            g_get_event(&g_event);

            switch(g_event.type) {
                case EVENT_KEY_PRESS:
                    game_on_key_press(&g_event.eventkey);
                    break;
                case EVENT_KEY_RELEASE:
                    game_on_key_release(&g_event.eventkey);
                    break;
                case EVENT_WINDOW_CLOSE:
                    g_done = TRUE;
                    break;
                case EVENT_WINDOW_RESIZE:
                    game_on_window_resize(&g_event.window);
                    break;
                case EVENT_MOUSE_MOVE:
                    game_on_mouse_move(&g_event.eventmouse);
                    break;
                case EVENT_MOUSE_PRESSED:
                    game_on_mouse_pressed(&g_event.eventmouse);
                    break;
                default: break;
            }
        }

        stoper_end(&stoper_dt);
        stoper_start(&stoper_dt);

        // handle rotation with mouse movement
        vec2f_sub(&g_mouse_pos, g_mouse_prev_pos);

        state.world->player->rotation.x += (-g_mouse_pos.y) / 5.f;
        state.world->player->rotation.y += (-g_mouse_pos.x) / 5.f;

        state.world->player->moving.up -= (g_free_cam ? 0 : 0.01f); // gravity
        if(state.world->player->moving.up < -0.2f) state.world->player->moving.up = -0.2f;

        world_tick(state.world);

        if(g_mouse_locked) {
            g_lock_mouse();

            g_mouse_pos = F2VEC2F(320, 240);
            g_mouse_prev_pos = g_mouse_pos;
        }

        /*snprintf(g_txt_position->text, 32, "Pos %.2f %.2f %.2f", g_world->player->position.x, g_world->player->position.y, g_world->player->position.z);
        text_update(g_txt_position, TEXT_UPDATE_STRING);

        snprintf(g_txt_rotation->text, 32, "Rot %.2f %.2f", g_world->player->rotation.x, g_world->player->rotation.y);
        text_update(g_txt_rotation, TEXT_UPDATE_STRING);

        snprintf(g_txt_title->text, 32, "FPS: %.2f", 1000.f / stoper_dt.delta);
        text_update(g_txt_title, TEXT_UPDATE_STRING);

        if(g_ray.valid) {
            snprintf(g_txt_looking->text, 32, "Looking at %d %d %d", g_ray.coord.x, g_ray.coord.y, g_ray.coord.z);
            text_update(g_txt_looking, TEXT_UPDATE_STRING);
        } else {
            snprintf(g_txt_looking->text, 32, "Looking at - - -");
            text_update(g_txt_looking, TEXT_UPDATE_STRING);
        }*/

        debugmenu_update(debugmenu);

        glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glDepthFunc(GL_LESS); 

        shader_use(state.shader_main);
        atlas_bind(state.atlas);
        world_render(state.world, state.shader_main);

        // render UI
        glDepthFunc(GL_ALWAYS); 

        shader_use(state.shader_ui);
        camera_use(g_cam_ui, state.shader_ui);

        sprite_render(g_spr_cursor, state.shader_ui);

        debugmenu_render(debugmenu);

        /*text_render(g_txt_title, g_shader_ui);
        text_render(g_txt_cam_mode, g_shader_ui);
        text_render(g_txt_block, g_shader_ui);
        text_render(g_txt_mode, g_shader_ui);
        text_render(g_txt_looking, g_shader_ui);
        text_render(g_txt_rotation, g_shader_ui);
        text_render(g_txt_position, g_shader_ui);*/

        g_swap_buffers();
    }
}
