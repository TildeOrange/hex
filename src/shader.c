#include <string.h>

#include "shader.h"

static char *vertex_header = 
"#define _VERTEX_\n"
"\n";

static char *geometry_header = 
"#define _GEOMETRY_\n"
"\n";

static char *fragment_header = 
"#define _FRAGMENT_\n"
"\n";

static GLuint _compile(GLenum type, const char *header, const char *source)
{
    GLuint handle;

    handle = glCreateShader(type);
    const char *sources[] = { "#version 330\n", header, source };
    glShaderSource(handle, array_length(sources), sources, NULL);
    glCompileShader(handle);

    GLint status;
    glGetShaderiv(handle, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE)
    {
        char log[1024];
        glGetShaderInfoLog(handle, sizeof(log), NULL, log);
        log_fatal_error("Shader Compilation Failed:\n%s", log);
    }

    return handle;
}

void shader_create(shader_t *self, const char *source)
{
    self->vs_handle = _compile(GL_VERTEX_SHADER, vertex_header, source);
    self->fs_handle = _compile(GL_FRAGMENT_SHADER, fragment_header, source);
    // @warning this has NOT been tested
    if (strstr(source, "_GEOMETRY_") != NULL)
        self->gs_handle = _compile(GL_GEOMETRY_SHADER, geometry_header, source);
    else
        self->gs_handle = 0;

    self->p_handle = glCreateProgram();

    glAttachShader(self->p_handle, self->vs_handle);
    glAttachShader(self->p_handle, self->fs_handle);
    if (self->gs_handle != 0)
        glAttachShader(self->p_handle, self->gs_handle);
    
    glLinkProgram(self->p_handle);

    GLint status;
    glGetProgramiv(self->p_handle, GL_LINK_STATUS, &status);
    if (status != GL_TRUE)
    {
        char log[1024];
        glGetProgramInfoLog(self->p_handle, sizeof(log), NULL, log);
        log_fatal_error("Program Linking Failed:\n%s", log);
    }
}

void shader_bind(shader_t *self)
{
    glUseProgram(self->p_handle);
}

void shader_get_attribute_indices(shader_t *self, vertex_attribute_t *attributes, u32 attribute_count)
{
    iterate_over_array(vertex_attribute_t, attribute, attributes, attribute_count)
        attribute->index = glGetAttribLocation(self->p_handle, attribute->name);
}