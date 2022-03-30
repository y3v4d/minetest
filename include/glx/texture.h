#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "utils/img_loader.h"

#include <GL/glew.h>
#include <GL/gl.h>

typedef struct {
    unsigned ID;

    unsigned width, height;
    int internal_format, format;
} texture_t;

texture_t* texture_make(const char *path);
texture_t* texture_make_from_bmp(bmp_t *bmp);

void texture_bind(const texture_t *p);
void texture_parameter(const texture_t *p, GLenum parameter, GLint value);

void texture_destroy(texture_t *p);

#endif