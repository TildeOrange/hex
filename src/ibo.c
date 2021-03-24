#include "ibo.h"

void ibo_create(ibo_t *self, GLsizeiptr size, const void *data, b32 is_dynamic)
{
    glGenBuffers(1, &self->handle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self->handle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, is_dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
}

void ibo_bind(ibo_t *self)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self->handle);
}

void *ibo_map(ibo_t *self)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self->handle);
    return glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_READ_WRITE);
}

void ibo_unmap(ibo_t *self)
{
    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
}

