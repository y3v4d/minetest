#include "glx/vbo.h"

#include <string.h>

vbo_t vbo_generate(GLuint type, bool_e dynamic) {
    vbo_t t = {
        .type = type,
        .dynamic = dynamic
    };

    glGenBuffers(1, &t.ID);

    return t;
}

void vbo_bind(vbo_t *p) {
    glBindBuffer(p->type, p->ID);
}

void vbo_data(vbo_t *p, size_t size, const void *data) {
    glBufferData(p->type, size, data, (p->dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
}

void vbo_destroy(vbo_t *p) {
    glDeleteBuffers(1, &p->ID);

    memset(p, 0, sizeof(vbo_t));
}