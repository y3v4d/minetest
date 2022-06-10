#ifndef __SHADER_H__
#define __SHADER_H__

#include <stdlib.h>

typedef enum {
    UNIFORM_MATRIX_4
} uniform_type_e;

typedef struct {
    unsigned vertex;
    unsigned fragment;

    unsigned program;   
} shader_t;

shader_t* shader_init(const char *path);
void shader_use(const shader_t *shader);
void shader_uniform(const shader_t *shader, const char *name, uniform_type_e t, size_t count, const void *data);
void shader_destroy(shader_t *shader);

#endif