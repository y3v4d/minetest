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

#define PI 3.14159265359
#define RADIANS(a) (a) * (180 / PI)

void GLAPIENTRY
MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity, message );
}

const float CUBE_VERTEX[] = {
    // front, right, back, left
    -0.5f,  0.5f,  0.5f, 0.0f, 1.0f,    // 0  left top front
     0.5f,  0.5f,  0.5f, 1.0f, 1.0f,    // 1  right top front
     0.5f, -0.5f,  0.5f, 1.0f, 0.0f,    // 2  right bottom front
    -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,    // 3  left bottom front

     0.5f,  0.5f, -0.5f, 0.0f, 1.0f,    // 4  right top back
    -0.5f,  0.5f, -0.5f, 1.0f, 1.0f,    // 5  left top back
    -0.5f, -0.5f, -0.5f, 1.0f, 0.0f,    // 6  left bottom back
     0.5f, -0.5f, -0.5f, 0.0f, 0.0f,    // 7  right bottom back

    // top
    -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,    // 8  left top back
     0.5f,  0.5f, -0.5f, 1.0f, 1.0f,    // 9  right top back
     0.5f,  0.5f,  0.5f, 1.0f, 0.0f,    // 10 right top front
    -0.5f,  0.5f,  0.5f, 0.0f, 0.0f,    // 11 left top front

    // bottom
    -0.5f, -0.5f,  0.5f, 0.0f, 1.0f,    // 12 left bottom front
     0.5f, -0.5f,  0.5f, 1.0f, 1.0f,    // 13 right bottom front
     0.5f, -0.5f, -0.5f, 1.0f, 0.0f,    // 14 right bottom back
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f     // 15 left bottom back
};

const unsigned int CUBE_INDICES[] = {
    // front
    0, 1, 2,
    2, 3, 0,

    // back
    4, 5, 6,
    6, 7, 4,
        
    // right
    1, 4, 7,
    7, 2, 1,

    // left
    5, 0, 3,
    3, 6, 5,

    // top
    8, 9, 10,
    10, 11, 8,

    // bottom
    12, 13, 14,
    14, 15, 12
};

const unsigned CHUNK_Z = 4, CHUNK_X = 4, CHUNK_Y = 1;

uint8_t chunk[16] = {
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1
};

// 1 vertex - 6 floats
// 1 face - 4 vertex
// 1 cube - 6 faces - 24 vertex - 144 floats

// 1 face - 6 indices
// 1 cube - 6 faces - 36 indices

float mesh[2304];
GLuint mesh_indices[576];
int mesh_index = 0;
int vertex_count = 0;
int indices_count = 0;

// direction 0 - front, 1 - back, 2 - right, 3 - left, 4 - top, 5 - bottom
void emit_face(int x, int y, int z, int direction) {
    // emit vertices
    for(int i = 0; i < 4; ++i) {
        const float *v = (i != 3 ? &CUBE_VERTEX[CUBE_INDICES[direction * 6 + i] * 5] : &CUBE_VERTEX[CUBE_INDICES[direction * 6 + 4] * 5]);

        mesh[mesh_index++] = x + v[0]; // position x
        mesh[mesh_index++] = y + v[1]; // position y
        mesh[mesh_index++] = z + v[2]; // position z
        mesh[mesh_index++] = v[3]; // uv.x
        mesh[mesh_index++] = v[4]; // uv.y

        if(direction == 4) mesh[mesh_index++] = 1;
        else if(direction == 5) mesh[mesh_index++] = 2;
        else mesh[mesh_index++] = 0;
    }

    for(int i = 0; i < 6; ++i) {
        mesh_indices[indices_count++] = vertex_count + CUBE_INDICES[i];
    }

    vertex_count += 4;
}

void make_mesh() {
    mesh_index    = 0;
    vertex_count  = 0;
    indices_count = 0;

    for(int i = 0; i < 4; ++i) {
        emit_face(i, 0, 0, 0);
        emit_face(i, 0, 0, 1);
        emit_face(i, 0, 0, 2);
        emit_face(i, 0, 0, 3);
        emit_face(i, 0, 0, 4);
        emit_face(i, 0, 0, 5);
    }
}

int main() {
    g_init();

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEPTH_TEST);
    glDebugMessageCallback(MessageCallback, 0);

    const GLubyte *version = glGetString(GL_VERSION);
    printf("OpenGL version: %s\n", version);

    make_mesh();
    
    unsigned int VBO, VAO, VEO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &VEO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh_index * sizeof(float), mesh, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VEO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_count * sizeof(GLuint), mesh_indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(5 * sizeof(float)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

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
        glBindVertexArray(VAO);

        glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_INT, (void*)0);

        g_swap_buffers();
    }

    close_shader(shader);
    g_close();

    return 0;
}
