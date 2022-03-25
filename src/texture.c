#include "texture.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void texture_destroy(texture_t *p) {
    if(!p) return;

    glDeleteTextures(1, &p->ID);
    free(p);
}

texture_t* texture_make_from_bmp(bmp_t *bmp) {
    if(!bmp) return NULL;

    texture_t *temp = (texture_t*)malloc(sizeof(texture_t));
    if(!temp) {
        fprintf(stderr, "Failed to allocate memory for texture\n");
        return NULL;
    }

    temp->width = bmp->width;
    temp->height = bmp->height;

    temp->internal_format = GL_RGBA;
    temp->format = GL_BGRA;

    glGenTextures(1, &temp->ID);
    glBindTexture(GL_TEXTURE_2D, temp->ID);

    glPixelStoref(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bmp->width, bmp->height, 0, GL_BGRA, GL_UNSIGNED_BYTE, bmp->data);

    glBindTexture(GL_TEXTURE_2D, 0);

    return temp;
}

texture_t* texture_make(const char *path) {
    bmp_t *img = load_bmp(path, 1, 0);
    if(!img) {
        fprintf(stderr, "Failed to make texture (image loading)\n");
        return NULL;
    }

    texture_t *tex = texture_make_from_bmp(img);
    free_bmp(img);

    return tex;
}

void texture_bind(const texture_t *p) {
    glBindTexture(GL_TEXTURE_2D, (!p ? 0 : p->ID));
}

void texture_parameter(const texture_t *p, GLenum parameter, GLint value) {
    texture_bind(p);
    glTexParameteri(GL_TEXTURE_2D, parameter, value);
}

