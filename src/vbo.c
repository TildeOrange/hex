#include "vbo.h"

void vbo_create(vbo_t *self, GLsizeiptr size, const void *data, b32 is_dynamic)
{
    glGenBuffers(1, &self->handle);
    glBindBuffer(GL_ARRAY_BUFFER, self->handle);
    glBufferData(GL_ARRAY_BUFFER, size, data, is_dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
}

void vbo_bind(vbo_t *self)
{
    glBindBuffer(GL_ARRAY_BUFFER, self->handle);
}

void *vbo_map(vbo_t *self)
{
    glBindBuffer(GL_ARRAY_BUFFER, self->handle);
    return glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
}

void vbo_unmap(vbo_t *self)
{
    glUnmapBuffer(GL_ARRAY_BUFFER);
}