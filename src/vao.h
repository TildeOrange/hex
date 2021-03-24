#ifndef _VAO_H
#define _VAO_H

#include "util.h"
#include "opengl.h"
#include "vbo.h"

#define MAX_ATTRIB_NAME_LEN 32
#define MAX_VAO_BINDINGS 2

typedef struct vertex_attribute_t
{
    char name[MAX_ATTRIB_NAME_LEN];
    GLuint binding;
    GLuint index;
    GLuint size;
    GLenum type;
    GLboolean normalized;
} vertex_attribute_t;

typedef struct vao_t
{
    GLuint handle;
    GLsizei strides[MAX_VAO_BINDINGS];
} vao_t;

void vao_create(vao_t *self, const vertex_attribute_t *attributes, u32 attribute_count);
void vao_bind(vao_t *self);
void vao_bind_buffer(vao_t *self, GLuint binding, vbo_t *vbo, GLuint divisor);

#endif