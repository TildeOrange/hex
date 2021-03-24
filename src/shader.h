#ifndef _SHADER_H
#define _SHADER_H

#include "util.h"
#include "opengl.h"
#include "vao.h"

typedef struct shader_t
{
    GLuint p_handle, vs_handle, gs_handle, fs_handle;
} shader_t;

void shader_create(shader_t *self, const char *source);
void shader_bind(shader_t *self);
void shader_get_attribute_indices(shader_t *self, vertex_attribute_t *attributes, u32 attribute_count);

#endif 