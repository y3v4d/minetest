#ifndef __VAO_H__
#define __VAO_H__

#include <GL/glew.h>
#include <GL/gl.h>

typedef struct {
    GLuint ID;
} vao_t;

vao_t vao_generate();
void vao_bind(vao_t *p);
void vao_attribute(GLuint index, GLint size, GLenum type, GLboolean normalized, size_t stride, const void *offset);
void vao_destroy(vao_t *p);

#endif