#include "text.h"
#include "glx/vao.h"
#include "glx/vbo.h"

#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GL/gl.h>

#define MAX_TEXT_LENGTH 32

const unsigned TEXT_INDICES[] = {
    0, 1, 2,
    3, 0, 2
};

text_t* text_make(fontbmp_t *font) {
    text_t *temp = (text_t*)malloc(sizeof(text_t));
    if(!temp) {
        fprintf(stderr, "Couldn't allocate memory for text_t\n");
        return NULL;
    }

    temp->font = font;

    temp->vertices = (float*)malloc(sizeof(float) * MAX_TEXT_LENGTH * 4 * 5);
    temp->indices = (unsigned*)malloc(sizeof(unsigned) * MAX_TEXT_LENGTH * 6);

    temp->vbo = vbo_generate(GL_ARRAY_BUFFER, TRUE);
    temp->vio = vbo_generate(GL_ELEMENT_ARRAY_BUFFER, TRUE);
    temp->vao = vao_generate();

    vao_bind(&temp->vao);
    vbo_bind(&temp->vbo);
    vbo_bind(&temp->vio);

    vao_attribute(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    vao_attribute(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    vao_bind(NULL);

    return temp;
}

void text_destroy(text_t *p) {
    if(!p) return;

    p->vertices_count = 0;
    p->indices_count = 0;

    free(p->vertices);
    free(p->indices);

    vbo_destroy(&p->vbo);
    vbo_destroy(&p->vio);
    vao_destroy(&p->vao);

    free(p);
}

void emit_character(text_t *p, char c, int *x, int *y) {
    fontbmp_t *font = p->font;
    charbmp_t *found = NULL;
    for(int i = 0; i < font->count; ++i) {
        if(font->chars[i].ID == c) {
            found = &font->chars[i];
            break;
        }
    }

    if(!found) {
        fprintf(stderr, "Couldn't find character %c\n", c);
        return;
    }

    // emit vertices
    float *v = p->vertices;

    v[p->mesh_counter++] = *x + found->xoff + 0.f;
    v[p->mesh_counter++] = *y + found->yoff + 0.f;
    v[p->mesh_counter++] = 0.f;
    v[p->mesh_counter++] = (float)found->x / font->texture->width;
    v[p->mesh_counter++] = 1.f - (float)found->y / font->texture->height;

    v[p->mesh_counter++] = *x + found->xoff + (float)found->w;
    v[p->mesh_counter++] = *y + found->yoff + 0.f;
    v[p->mesh_counter++] = 0.f;
    v[p->mesh_counter++] = (float)(found->x + found->w) / font->texture->width;
    v[p->mesh_counter++] = 1.f - (float)found->y / font->texture->height;

    v[p->mesh_counter++] = *x + found->xoff + (float)found->w;
    v[p->mesh_counter++] = *y + found->yoff + (float)found->h;
    v[p->mesh_counter++] = 0.f;
    v[p->mesh_counter++] = (float)(found->x + found->w) / font->texture->width;
    v[p->mesh_counter++] = 1.f - (float)(found->y + found->h) / font->texture->height;

    v[p->mesh_counter++] = *x + found->xoff + 0.f;
    v[p->mesh_counter++] = *y + found->yoff + (float)found->h;
    v[p->mesh_counter++] = 0.f;
    v[p->mesh_counter++] = (float)found->x / font->texture->width;
    v[p->mesh_counter++] = 1.f - (float)(found->y + found->h) / font->texture->height;

    // emit indices
    for(int i = 0; i < 6; ++i) {
        p->indices[p->indices_count++] = p->vertices_count + TEXT_INDICES[i];
    }

    p->vertices_count += 4;

    *x += found->xadv;
}

void text_set(text_t *p, const char *string) {
    p->text = string;

    p->vertices_count = 0;
    p->indices_count = 0;
    p->mesh_counter = 0;

    int x = 0, y = 0;
    for(int i = 0; string[i] != 0; ++i) {
        emit_character(p, string[i], &x, &y);
    }

    vao_bind(&p->vao);
    vbo_bind(&p->vbo);
    vbo_data(&p->vbo, p->mesh_counter * sizeof(float), p->vertices);
    vbo_bind(&p->vio);
    vbo_data(&p->vio, p->indices_count * sizeof(unsigned), p->indices);
}

void text_render(text_t *p) {
    vao_bind(&p->vao);
    vbo_bind(&p->vbo);
    vbo_bind(&p->vio);

    glActiveTexture(GL_TEXTURE0);
    texture_bind(p->font->texture);

    glDrawElements(GL_TRIANGLES, p->indices_count, GL_UNSIGNED_INT, (void*)0);
}