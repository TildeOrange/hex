#ifndef _MESH_H
#define _MESH_H

#include <stdio.h>
#include "util.h"
#include "vectors.h"
#include "opengl.h"

typedef struct mesh_vertex_t
{
    vector3f position;
    vector3f normal;
} mesh_vertex_t;

typedef struct mesh_t
{
    u16 *indices;
    mesh_vertex_t *vertices;
    u32 index_count;
    u32 vertex_count;
} mesh_t;

void mesh_load_wavefront(mesh_t *self, FILE *stream);

/*
typedef struct renderable_mesh_t
{
    vbo_t vbo;
    ibo_t ibo;
    u32 index_count;
} renderable_mesh_t;
*/

#endif 