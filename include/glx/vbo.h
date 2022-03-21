#ifndef __VBO_H__
#define __VBO_H__

#include <GL/glew.h>
#include <GL/glx.h>

typedef enum { FALSE, TRUE } bool_e;

typedef struct {
    GLuint ID;
    GLenum type;
    bool_e dynamic;
} vbo_t;

vbo_t vbo_generate(GLuint type, bool_e dynamic);
void vbo_bind(vbo_t *p);
void vbo_data(vbo_t *p, size_t size, const void *data);
void vbo_destroy(vbo_t *p);

#endif