#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <GL/glew.h>
#include <GL/glx.h>

#include "system.h"
#include "shader.h"
#include "event.h"

#include "utils/img_loader.h"

#include <math.h>

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

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    byte_t data[3 * 4] = {
        255.0f, 0.0f, 0.0f, 0.0f, 255.0f, 0.0f,
        0.0f, 0.0f, 255.0f, 0.0f, 0.0f, 0.0f
    };

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, img->width, img->height);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, img->width, img->height, GL_BGR, GL_UNSIGNED_BYTE, img->data);

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
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, (void*)0);

        g_swap_buffers();
    }

    close_shader(shader);
    g_close();

    return 0;
}
