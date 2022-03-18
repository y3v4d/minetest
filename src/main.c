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

#define PI 3.14159265359
#define RADIANS(a) (a) * (180 / PI)

void GLAPIENTRY
MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity, message );
}

void set_rotation_z(float angle, float m[]) {
    m[0] = cosf(RADIANS(angle));
    m[1] = -sinf(RADIANS(angle));
    m[4] = sinf(RADIANS(angle));
    m[5] =  cosf(RADIANS(angle));
}

void set_rotation_y(float angle, float m[]) {
    m[0] = cosf(RADIANS(angle));
    m[2] = sinf(RADIANS(angle));
    m[8] = -sinf(RADIANS(angle));
    m[10] = cosf(RADIANS(angle));
}

void set_rotation_x(float angle, float m[]) {
    m[5]  = cosf(RADIANS(angle));
    m[6]  = -sinf(RADIANS(angle)); 
    m[9]  = sinf(RADIANS(angle));
    m[10] = cosf(RADIANS(angle));
}

int main() {
    g_init();

    float vertices[] = {
        -1.0f, 1.0f, 1.0f, 0.0f, 1.0f,      // 0 left top front
        1.0f, 1.0f, 1.0f, 1.0f, 1.0f,       // 1 right top front
        1.0f, -1.0f, 1.0f, 1.0f, 0.0f,      // 2 right bottom front
        -1.0f, -1.0f, 1.0f, 0.0f, 0.0f,     // 3 left bottom front

        1.0f, 1.0f, -1.0f, 0.0f, 1.0f,      // 4 right top back
        -1.0f, 1.0f, -1.0f, 1.0f, 1.0f,     // 5 left top back
        -1.0f, -1.0f, -1.0f, 1.0f, 0.0f,    // 6 left bottom back
        1.0f, -1.0f, -1.0f, 0.0f, 0.0f      // 7 right bottom back
    };

    unsigned int indices[] = {
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
        5, 4, 1,
        1, 0, 5,

        // bottom
        3, 2, 7,
        7, 6, 3
    };

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEPTH_TEST);
    glDebugMessageCallback(MessageCallback, 0);

    const GLubyte *version = glGetString(GL_VERSION);
    printf("OpenGL version: %s\n", version);
    
    unsigned int VBO, VAO, VEO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &VEO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VEO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

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

    // setup perspective projection
    float near = 0.1f, far = 100.0f;

    const float FOV = 60.0f;
    float top = tanf(RADIANS(FOV / 2.0f)) * near;
    float bottom = -top;

    float aspect_ratio = 640.f / 480.f;
    float right = top * aspect_ratio;
    float left = -right;

    float projection[16] = {
        (2.0f * near) / (right - left), 0.0f, (right + left) / (right - left), 0.0f,
        0.0f, (2.0f * near) / (top - bottom), (top + bottom) / (top - bottom), 0.0f,
        0.0f, 0.0f, -(far + near) / (far - near), -(2.0f * far * near) / (far - near),
        0.0f, 0.0f, -1.0f, 1.0f
    };

    float model[16] = {
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, -4.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    float rotation_y[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    float rotation_z[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    GLint matrix_location = glGetUniformLocation(shader->program, "matrix");
    GLint model_location = glGetUniformLocation(shader->program, "model");
    GLint layer_location = glGetUniformLocation(shader->program, "layer");
    GLint rotation_y_location = glGetUniformLocation(shader->program, "rotation_y");
    GLint rotation_z_location = glGetUniformLocation(shader->program, "rotation_z");

    const float SPEED = 0.1f;
    const float ROTATION_SPEED = 0.001f;
    float v_x = 0.0f;
    float v_y = 0.0f;
    float v_z = 0.0f;

    float angle_y = 0.f;
    float angle_z = 0.f;
    float v_ay = 0.0f;
    float v_az = 0.0f;

    int layer = 0;

    // event loop
    event_t event;
    short done = 0;
    while(!done) {
        while(g_pending_events()) {
            g_get_event(&event);

            if(event.type == EVENT_KEY_PRESS) {
                char key = event.eventkey.key;

                switch(key) {
                    case 'q': done = 1; break;
                    case 'r':
                        close_shader(shader);
                        shader = make_shader("data/shaders/main");

                        break;
                    case 's': v_az = -ROTATION_SPEED; break;
                    case 'w': v_az = ROTATION_SPEED; break;
                    case 'a': v_ay = ROTATION_SPEED; break;
                    case 'd': v_ay = -ROTATION_SPEED; break;
                    case 't': v_y = -SPEED; break;
                    case 'g': v_y = SPEED; break;
                    default: break;
                }

                if(key >= '1' && key <= '4') {
                    layer = key - '0' - 1;
                }
            } else if(event.type == EVENT_KEY_RELEASE) {
                char key = event.eventkey.key;

                if(key == 's' || key == 'w') v_az = 0;
                else if(key == 'a' || key == 'd') v_ay = 0;
                else if(key == 't' || key == 'g') v_y = 0;
            } else if(event.type == EVENT_WINDOW_CLOSE) {
                printf("WM_DELETE_WINDOW invoked\n");
                done = 1;
            }
        }

        model[3] += v_x;
        model[7] += v_y;
        model[11] += v_z;

        angle_y += v_ay;
        angle_z += v_az;

        set_rotation_y(-angle_y, rotation_y);
        set_rotation_x(-angle_z, rotation_z);

        glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader->program);

        glUniformMatrix4fv(matrix_location, 1, GL_TRUE, projection);
        glUniformMatrix4fv(model_location, 1, GL_TRUE, model);
        glUniformMatrix4fv(rotation_y_location, 1, GL_TRUE, rotation_y);
        glUniformMatrix4fv(rotation_z_location, 1, GL_TRUE, rotation_z);
        glUniform1i(layer_location, layer);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, (void*)0);

        g_swap_buffers();
    }

    close_shader(shader);
    g_close();

    return 0;
}
