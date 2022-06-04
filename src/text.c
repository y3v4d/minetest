#include "text.h"

#include "glx/shader.h"
#include "glx/vao.h"
#include "glx/vbo.h"
#include "mesh.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GL/glew.h>
#include <GL/glx.h>

const unsigned TEXT_INDICES[] = {
    0, 1, 2,
    3, 0, 2
};

text_t* text_make(fontbmp_t *font, const char *string, vec3f position) {
    text_t *temp = (text_t*)malloc(sizeof(text_t));
    if(!temp) {
        fprintf(stderr, "Couldn't allocate memory for text_t\n");
        return NULL;
    }

    temp->font = font;

    temp->mesh = mesh_init(MAX_TEXT_LENGTH * 4 * 5, MAX_TEXT_LENGTH * 6);

    vao_bind(&temp->mesh->vao);
    vbo_bind(&temp->mesh->vbo);
    vbo_bind(&temp->mesh->vio);

    vao_attribute(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    vao_attribute(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    
    vao_bind(NULL);

    strncpy(temp->text, string, MAX_TEXT_LENGTH);
    text_update(temp, TEXT_UPDATE_ALL);

    temp->position = position;
    temp->model = mat4_translation(position);

    return temp;
}

void text_destroy(text_t *p) {
    if(!p) return;

    mesh_destroy(p->mesh);

    free(p);
}

void emit_character(text_t *p, char c, int *x, int *y) {
    if(c == '\n') {
        *x = 0;
        *y += 32.f;
        return;
    }

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
    mesh_t *mesh = p->mesh;
    float *vd = (float*)mesh->vertices->data;

    vd[mesh->vertex_counter++] = *x + found->xoff + 0.f;
    vd[mesh->vertex_counter++] = *y + found->yoff + 0.f;
    vd[mesh->vertex_counter++] = 0.f;
    vd[mesh->vertex_counter++] = (float)found->x / font->texture->width;
    vd[mesh->vertex_counter++] = 1.f - (float)found->y / font->texture->height;

    vd[mesh->vertex_counter++] = *x + found->xoff + (float)found->w;
    vd[mesh->vertex_counter++] = *y + found->yoff + 0.f;
    vd[mesh->vertex_counter++] = 0.f;
    vd[mesh->vertex_counter++] = (float)(found->x + found->w) / font->texture->width;
    vd[mesh->vertex_counter++] = 1.f - (float)found->y / font->texture->height;

    vd[mesh->vertex_counter++] = *x + found->xoff + (float)found->w;
    vd[mesh->vertex_counter++] = *y + found->yoff + (float)found->h;
    vd[mesh->vertex_counter++] = 0.f;
    vd[mesh->vertex_counter++] = (float)(found->x + found->w) / font->texture->width;
    vd[mesh->vertex_counter++] = 1.f - (float)(found->y + found->h) / font->texture->height;

    vd[mesh->vertex_counter++] = *x + found->xoff + 0.f;
    vd[mesh->vertex_counter++] = *y + found->yoff + (float)found->h;
    vd[mesh->vertex_counter++] = 0.f;
    vd[mesh->vertex_counter++] = (float)found->x / font->texture->width;
    vd[mesh->vertex_counter++] = 1.f - (float)(found->y + found->h) / font->texture->height;

    // emit indices
    for(int i = 0; i < 6; ++i) {
        ((unsigned*)mesh->indices->data)[mesh->indices->index++] = mesh->vertices->index + TEXT_INDICES[i];
    }

    mesh->vertices->index += 4;

    *x += found->xadv;
}

void text_update(text_t *p, uint32_t flag) {
    if(flag & TEXT_UPDATE_POSITION) {
        mat4_translate(&p->model, p->position);
    }
    if(flag & TEXT_UPDATE_STRING) {
        mesh_prepare(p->mesh);

        int x = 0, y = 0;
        for(int i = 0; p->text[i] != 0 && i < MAX_TEXT_LENGTH; ++i) {
            emit_character(p, p->text[i], &x, &y);
        }

        mesh_finalize(p->mesh);
    }
}

void text_render(text_t *p, const shader_t *shader) {
    shader_uniform(shader, "model", UNIFORM_MATRIX_4, 1, p->model.m);

    glActiveTexture(GL_TEXTURE0);
    texture_bind(p->font->texture);

    vao_bind(&p->mesh->vao);
    glDrawElements(GL_TRIANGLES, p->mesh->indices->index, GL_UNSIGNED_INT, (void*)0);
}