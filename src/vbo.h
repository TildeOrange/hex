#ifndef _VBO_H
#define _VBO_H

#include "util.h"
#include "opengl.h"

typedef struct vbo_t
{
    GLuint handle;
} vbo_t;

void vbo_create(vbo_t *self, GLsizeiptr size, const void *data, b32 is_dynamic);
void vbo_bind(vbo_t *self);
void *vbo_map(vbo_t *self);
void vbo_unmap(vbo_t *self);

#endif