#include "atlas.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utils/img_loader.h"

#include <GL/glew.h>
#include <GL/glx.h>

typedef unsigned char byte_t;

atlas_t* atlas_generate(const char *path, unsigned tile_w, unsigned tile_h) {
    atlas_t *t = (atlas_t*)malloc(sizeof(atlas_t));
    if(!t) {
        printf("Couldn't allocate memory for atlas\n");
        return NULL;
    }

    t->tile_w = tile_w;
    t->tile_h = tile_h;

    bmp_t *img = load_bmp(path, 1, 1);
    if(!img) {
        fprintf(stderr, "Couldn't load texture image %s\n", path);
        return NULL;
    }

    glGenTextures(1, &t->ID);
    glBindTexture(GL_TEXTURE_2D_ARRAY, t->ID);

    glPixelStoref(GL_UNPACK_ALIGNMENT, 1);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, t->tile_w, t->tile_h, 9);

    printf("Atlas tiles %d %d\n", img->width / t->tile_w, img->height / t->tile_h);

    const unsigned BPP = 4;
    byte_t tile_buffer[t->tile_w * t->tile_h * BPP];
    byte_t *start = img->data;
    for(int y = 0; y < img->height / t->tile_h; ++y) {
        for(int x = 0; x < img->width / t->tile_w; ++x) {
            start = img->data + ((y * t->tile_h * img->width) + t->tile_w * x) * BPP;

            // flip individual images vertically
            for(int row = 0; row < t->tile_h; ++row) {
                memcpy(tile_buffer + row * t->tile_w * BPP, start + (tile_h - 1 - row) * img->width * BPP, t->tile_w * BPP);
            }

            glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, (y * (img->height / t->tile_h) + x), t->tile_w, t->tile_h, 1, GL_BGRA, GL_UNSIGNED_BYTE, tile_buffer);
        }
    }

    free_bmp(img);

    return t;
}

void atlas_bind(atlas_t *p) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, p->ID);
}

void atlas_destroy(atlas_t *p) {
    glDeleteTextures(1, &p->ID);

    memset(p, 0, sizeof(atlas_t));
}