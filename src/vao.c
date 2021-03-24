#include <string.h>
#include "vao.h"

static GLsizei _get_size(const vertex_attribute_t *attribute)
{
    switch (attribute->type)
    {
        case GL_BYTE:           return 1 * attribute->size;
        case GL_UNSIGNED_BYTE:  return 1 * attribute->size;
        case GL_SHORT:          return 2 * attribute->size;
        case GL_UNSIGNED_SHORT: return 2 * attribute->size;
        case GL_INT:            return 4 * attribute->size;
        case GL_UNSIGNED_INT:   return 4 * attribute->size;
        case GL_FLOAT:          return 4 * attribute->size;
        default: 
            log_error("Unrecognized OpenGL type enum %d in attribute[%s]", attribute->type, attribute->name); 
            return 0;
    }
}

void vao_create(vao_t *self, const vertex_attribute_t *attributes, u32 attribute_count)
{
    glGenVertexArrays(1, &self->handle);
    glBindVertexArray(self->handle);

    memset(self->strides, 0, sizeof(GLsizei) * MAX_VAO_BINDINGS);

    iterate_over_array(const vertex_attribute_t, attribute, attributes, attribute_count)
    {
        glEnableVertexAttribArray(attribute->index);
        glVertexAttribBinding(attribute->index, attribute->binding);
        glVertexAttribFormat(attribute->index, attribute->size, attribute->type, attribute->normalized, self->strides[attribute->binding]);
        self->strides[attribute->binding] += _get_size(attribute);
    }
}

void vao_bind(vao_t *self)
{
    glBindVertexArray(self->handle);
}

void vao_bind_buffer(vao_t *self, GLuint binding, vbo_t *vbo, GLuint divisor)
{
    glVertexBindingDivisor(binding, divisor);
    glBindVertexBuffer(binding, vbo->handle, 0, self->strides[binding]);
}