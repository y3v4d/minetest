#ifndef __SHADER_H__
#define __SHADER_H__

typedef struct {
    unsigned vertex;
    unsigned fragment;

    unsigned program;   
} shader_t;

shader_t* make_shader(const char *path);
void close_shader(shader_t *shader);

#endif