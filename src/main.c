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

void GLAPIENTRY
MessageCallback( GLenum source,
                         GLenum type,
                                          GLuint id,
                                                           GLenum severity,
                                                                            GLsizei length,
                                                                                             const GLchar* message,
                                                                                                              const void* userParam )
{
      fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
                         ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
                                     type, severity, message );
}

int main() {
    g_init();

    float vertices[] = {
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    glEnable(GL_DEBUG_OUTPUT);
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

    bmp_t *img = load_bmp("data/textures/atlas.bmp");
    if(!img) {
        fprintf(stderr, "Couldn't load texture!\n");
        return 1;
    }

    byte_t data[2 * 3] = {
        255.f, 0.f, 0.f,
        255.f, 0.f, 0.f
    };

    unsigned int texture;

    /*GLsizei width = 1;
    GLsizei height = 1;
    GLsizei layerCount = 1;
    GLsizei mipLevelCount = 1;

    GLubyte texels[32] = {
        // Texels for first image.
        0,   0,   255,   255,
        255, 0,   0,   255,
        0,   255, 0,   255,
        0,   0,   255, 255,
        // Texels for second image.
        255, 255, 255, 255,
        255, 255,   0, 255,
        0,   255, 255, 255,
        255, 0,   255, 255,
    };*/

    const unsigned TILE_W = 16, TILE_H = 16;

    glGenTextures(1,&texture);
    glBindTexture(GL_TEXTURE_2D_ARRAY,texture);

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

    //glTexStorage3D(GL_TEXTURE_2D_ARRAY, mipLevelCount, GL_RGBA8, width, height, layerCount);
    //glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, width, height, layerCount, GL_RGBA, GL_UNSIGNED_BYTE, texels);

    /*glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGB8, 1, 1, 2);
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, 1, 1, 2, GL_RGB, GL_UNSIGNED_BYTE, data);*/
    /*glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, img->width, img->height);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, img->width, img->height, GL_BGR, GL_UNSIGNED_BYTE, img->data);*/

    free_bmp(img);

    float near = 0.1f, far = 100.0f;

    const float FOV = 60.0f;
    const float PI = 3.14159265359f;
    float top = tanf(FOV / 2.0f * (180.0f / PI)) * near;
    float bottom = -top;

    float aspect_ratio = 640.f / 480.f;
    float right = top * aspect_ratio;
    float left = -right;

    float matrix[16] = {
        (2.0f * near) / (right - left), 0.0f, (right + left) / (right - left), 0.0f,
        0.0f, (2.0f * near) / (top - bottom), (top + bottom) / (top - bottom), 0.0f,
        0.0f, 0.0f, -(far + near) / (far - near), -(2.0f * far * near) / (far - near),
        0.0f, 0.0f, -1.0f, 1.0f
    };

    float model[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, -2.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    GLint matrix_location = glGetUniformLocation(shader->program, "matrix");
    GLint model_location = glGetUniformLocation(shader->program, "model");

    const float SPEED = 0.1f;
    float v_x = 0.0f;
    float v_z = 0.0f;

    // event loop
    event_t event;
    short done = 0;
    while(!done) {
        while(g_pending_events()) {
            g_get_event(&event);

            if(event.type == EVENT_KEY_PRESS) {
                char key = event.eventkey.key;

                if(key == 'q') {
                    done = 1;
                } else if(key == 'r') {
                    close_shader(shader);
                    shader = make_shader("data/shaders/main");
                } else if(key == 's') {
                    v_z = -SPEED;
                } else if(key == 'w') {
                    v_z = SPEED;
                } else if(key == 'a') {
                    v_x = SPEED;
                } else if(key == 'd') {
                    v_x = -SPEED;
                }
            } else if(event.type == EVENT_KEY_RELEASE) {
                char key = event.eventkey.key;

                if(key == 's' || key == 'w') v_z = 0;
                else if(key == 'a' || key == 'd') v_x = 0;
            } else if(event.type == EVENT_WINDOW_CLOSE) {
                printf("WM_DELETE_WINDOW invoked\n");
                done = 1;
            }
        }

        model[3] += v_x;
        model[11] += v_z;

        glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader->program);

        glUniformMatrix4fv(matrix_location, 1, GL_TRUE, matrix);
        glUniformMatrix4fv(model_location, 1, GL_TRUE, model);

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
