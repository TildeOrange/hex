#include "mesh.h"
#include "mem.h"

typedef struct vertex_index_t
{
    u16 position_index;
    // u16 texcoord_index;
    u16 normal_index;
} vertex_index_t;

void mesh_load_wavefront(mesh_t *self, FILE *stream)
{
    // @todo handle texture coords too
    size_t position_list_start = 0, normal_list_start = 0, face_list_start = 0;
    u32 position_count = 0, normal_count = 0, face_count = 0, unique_vertex_count = 0;
    vector3f *position_pool, *normal_pool;
    vertex_index_t *vertex_pool, *unique_vertices;
    char buffer[1024];

    // count positions, texcoords, normals, and faces
    {
        while (!feof(stream))
        {
            size_t pos = ftell(stream) - 1;

            int res = fscanf(stream, "%s", buffer);

            if (strcmp("v", buffer) == 0)
            {
                position_count++;
                if (position_list_start == 0)
                    position_list_start = pos;
            }
            else if (strcmp("vn", buffer) == 0)
            {
                normal_count++;
                if (normal_list_start == 0)
                    normal_list_start = pos;
            }
            else if (strcmp("f", buffer) == 0)
            {
                face_count++;
                if (face_list_start == 0)
                    face_list_start = pos;
            }

            // printf("[0] buffer=%s\n", buffer);

            fgets(buffer, 1024, stream); // skip rest of line
        }
    }

    // read positions, texcoords, normals, and vertex instances from the stream
    {

        position_pool = temp_alloc(sizeof(vector3f) * position_count);
        normal_pool = temp_alloc(sizeof(vector3f) * normal_count);
        vertex_pool = temp_alloc(sizeof(vertex_index_t) * face_count * 3);

        fseek(stream, position_list_start, SEEK_SET);
        iterate_over_array(vector3f, position, position_pool, position_count)
        {
            fgets(buffer, 1024, stream);
            if (fscanf(stream, "v %f %f %f", &position->x, &position->y, &position->z) != 3)
            {
                log_error("Failed to read a position in a waveforms file\n");
                continue;
            }

        }

        fseek(stream, normal_list_start, SEEK_SET);
        iterate_over_array(vector3f, normal, normal_pool, normal_count)
        {
            fgets(buffer, 1024, stream);
            if (fscanf(stream, "vn %f %f %f", &normal->x, &normal->y, &normal->z) != 3)
            {
                log_error("Failed to read a normal in a waveforms file\n");
                continue;
            }

        }

        fseek(stream, face_list_start, SEEK_SET);
        for (int i = 0; i < face_count; i++)
        {
            vertex_index_t *v[3] = { vertex_pool + i * 3, vertex_pool + i * 3 + 1, vertex_pool + i * 3 + 2 };

            fgets(buffer, 1024, stream);
            if (fscanf(stream, "f %hu/%*hu/%hu %hu/%*hu/%hu %hu/%*hu/%hu", 
                        &v[0]->position_index, &v[0]->normal_index,
                        &v[1]->position_index, &v[1]->normal_index,
                        &v[2]->position_index, &v[2]->normal_index) != 6)
            {
                log_error("Failed to read a face in a waveforms file\n");
                continue;
            }

            // decrement by one because wavefront indices start at 1
            v[0]->position_index -= 1;
            v[0]->normal_index -= 1;
            v[1]->position_index -= 1;
            v[1]->normal_index -= 1;
            v[2]->position_index -= 1;
            v[2]->normal_index -= 1;

        }

    }

    // gather unique vertices
    {
        unique_vertices = temp_alloc(sizeof(vertex_index_t) * face_count * 3);

        iterate_over_array(vertex_index_t, vertex, vertex_pool, face_count * 3)
        {
            // @speedup
            b32 is_unique = true;
            for (int i = 0; i < unique_vertex_count; i++)
            iterate_over_array(vertex_index_t, unique_vertex, unique_vertices, unique_vertex_count)
            {
                if (vertex->position_index == unique_vertex->position_index && vertex->normal_index == unique_vertex->normal_index)
                {
                    is_unique = false;
                    break;
                }
            }

            if (is_unique)
            {
                unique_vertices[unique_vertex_count++] = *vertex;
            }
        }
    }

    // make vbo from unique vertices
    {
        self->vertex_count = unique_vertex_count;
        self->vertices = perm_alloc(self->vertex_count * sizeof(mesh_vertex_t));
        for (int i = 0; i < unique_vertex_count; i++)
        {
            const vertex_index_t *vertex = unique_vertices + i;
            const vector3f *position = position_pool + vertex->position_index;
            const vector3f *normal = normal_pool + vertex->normal_index;

            mesh_vertex_t *mesh_vertex = self->vertices + i;
            mesh_vertex->position = *position;
            mesh_vertex->normal = *normal;
        }
    }

    // make ibo from faces
    {
        self->index_count = face_count * 3;
        self->indices = perm_alloc(sizeof(u16) * self->index_count);

        u16 *index = self->indices;
        iterate_over_array(const vertex_index_t, vertex, vertex_pool, face_count * 3)
        {
            iterate_over_array(vertex_index_t, unique_vertex, unique_vertices, unique_vertex_count)
            {
                if (vertex->position_index == unique_vertex->position_index && vertex->normal_index == unique_vertex->normal_index)
                {
                    *(index++) = (u16) (unique_vertex - unique_vertices);
                    break;
                }
            }
        }
    }

    temp_free(unique_vertices);
    temp_free(vertex_pool);
    temp_free(normal_pool);
    temp_free(position_pool);

    // debug printing
    {
        printf("found %d positions at %zu, %d normals at %zu, and %d unique vertices\n", position_count, position_list_start, normal_count, normal_list_start, unique_vertex_count);

        // fseek(stream, position_list_start, SEEK_SET);
        // fread(buffer, 1, 29, stream);
        // buffer[29] = 0;
        // printf("buffer=%s\n", buffer);

        // fseek(stream, normal_list_start, SEEK_SET);
        // fread(buffer, 1, 24, stream);
        // buffer[24] = 0;
        // printf("buffer=%s\n", buffer);

        // iterate_over_array(vector3f, position, position_pool, position_count)
        // {
        //     printf("v %f %f %f\n", position->x, position->y, position->z);
        // }
        // printf("\n");
        // iterate_over_array(vector3f, normal, normal_pool, normal_count)
        // {
        //     printf("vn %f %f %f\n", normal->x, normal->y, normal->z);
        // }
        // iterate_over_array(vertex_index_t, vertex, vertex_pool, face_count * 3)
        // {
        //     printf("vertex %hu/%hu\n", vertex->position_index, vertex->normal_index);
        // }
        // iterate_over_array(vertex_index_t, vertex, unique_vertices, unique_vertex_count)
        // {
        //     printf("unique vertex %hu/%hu\n", vertex->position_index, vertex->normal_index);
        // }
    }
}