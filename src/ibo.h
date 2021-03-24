#ifndef _IBO_H
#define _IBO_H

#include "util.h"
#include "opengl.h"

typedef struct ibo_t
{
    GLuint handle;
} ibo_t;

void ibo_create(ibo_t *self, GLsizeiptr size, const void *data, b32 is_dynamic);
void ibo_bind(ibo_t *self);
void *ibo_map(ibo_t *self);
void ibo_unmap(ibo_t *self);

#endif