#include "glx/vao.h"

#include <string.h>

vao_t vao_generate() {
    vao_t t;

    glGenVertexArrays(1, &t.ID);

    return t;
}

void vao_bind(vao_t *p) {
    if(!p) glBindVertexArray(0);
    else glBindVertexArray(p->ID);
}

void vao_attribute(GLuint index, GLint size, GLenum type, GLboolean normalized, size_t stride, const void *offset) {
    glVertexAttribPointer(
        index, 
        size, 
        type, 
        normalized, 
        stride, 
        offset
    );

    glEnableVertexAttribArray(index);
}

void vao_destroy(vao_t *p) {
    glDeleteVertexArrays(1, &p->ID);

    memset(p, 0, sizeof(vao_t));
}