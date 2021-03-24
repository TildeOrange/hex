#include "util.h"
#include "opengl.h"
#include "shader.h"
#include "vao.h"
#include "vbo.h"
#include "ibo.h"
#include "camera.h"
#include "world.h"
#include "mesh.h"
#include "config.h"
#include "res.h"
#include "mem.h"
#include "window.h"

#define FOV_DEG 70.0f

// global config (from config.h)
config_t config;

static struct tile_meshes_t {
    mesh_t default_mesh;
    vbo_t default_vbo;
    ibo_t default_ibo;
    mesh_t grass_tile_mesh;
    vbo_t grass_tile_vbo;
    ibo_t grass_tile_ibo;
} tile_meshes;

static shader_t shader;
static vao_t vao;
static vbo_t instances_vbo;
static camera_t camera;
static GLuint view_index, proj_index, color_index, time_index, base_index_index, selected_instance_index;
static world_t world;
static b32 cursor_locked;

// @speedup this is terrible if we have to move 12 bytes, it would be a billion times better if we could index instances from the instance buffer
// and break the world into chucks, but for now no.
#if 0
// static vbo_t visible_instances_vbo;
// static u32 visible_instances_type_start[TILE_TYPE_COUNT], visible_instances_type_count[TILE_TYPE_COUNT];
static void world_load_visible_tiles(world_t *world, vbo_t *src, vbo_t *dst, const camera_t *cam)
{
    glBindBuffer(GL_COPY_READ_BUFFER, src->handle);
    glBindBuffer(GL_COPY_WRITE_BUFFER, dst->handle);

    const vector3f *src_vectors = glMapBuffer(GL_COPY_READ_BUFFER, GL_READ_ONLY);
    vector3f *dst_vectors = glMapBuffer(GL_COPY_WRITE_BUFFER, GL_WRITE_ONLY);

    u32 prev_start = 0;
    for (int i = 0; i < TILE_TYPE_COUNT; i++)
    {
        u32 src_start = world->type_start[i]; 
        u32 src_count = world->type_count[i]; 
        visible_instances_type_count[i] = 0;

        for (int j = src_start; j < src_start + src_count; j++)
        {
            // -- t --
            // |     |
            // l     r
            // |     |
            // -- b --

            const vector3f *src_vector = src_vectors + j;
            vector3f dir;
            v3f_sub(src_vector, &cam->position, &dir);
            f32 dist = v3f_mag(&dir);
            if (dist < 0.0f)
                continue;

            if (v3f_dot(&camera.forward, &dir) < dist * cosf(config.fov - rad(25)))
                continue;

            dst_vectors[prev_start + visible_instances_type_count[i]++] = *src_vector;
        }

        visible_instances_type_start[i] = prev_start;
        prev_start = visible_instances_type_start[i] + visible_instances_type_count[i];
    }

    glUnmapBuffer(GL_COPY_READ_BUFFER);
    glUnmapBuffer(GL_COPY_WRITE_BUFFER);
}
#endif

static void _init()
{
    memory_create(MB(16), MB(16));

    srand(config.seed);

    FILE *mesh_file = fopen("res/hexagon.obj", "rb");
    mesh_load_wavefront(&tile_meshes.default_mesh, mesh_file);
    fclose(mesh_file);
    
    mesh_file = fopen("res/grass_tile.obj", "rb");
    mesh_load_wavefront(&tile_meshes.grass_tile_mesh, mesh_file);
    fclose(mesh_file);

    world_create(&world, config.world_radius);

    vbo_create(&tile_meshes.default_vbo, sizeof(mesh_vertex_t) * tile_meshes.default_mesh.vertex_count, tile_meshes.default_mesh.vertices, false);
    ibo_create(&tile_meshes.default_ibo, sizeof(u16) * tile_meshes.default_mesh.index_count, tile_meshes.default_mesh.indices, false);
    vbo_create(&tile_meshes.grass_tile_vbo, sizeof(mesh_vertex_t) * tile_meshes.grass_tile_mesh.vertex_count, tile_meshes.grass_tile_mesh.vertices, false);
    ibo_create(&tile_meshes.grass_tile_ibo, sizeof(u16) * tile_meshes.grass_tile_mesh.index_count, tile_meshes.grass_tile_mesh.indices, false);

    vbo_create(&instances_vbo, sizeof(vector3f) * world.tile_count, world.vbo_coords, false);
    // vbo_create(&visible_instances_vbo, sizeof(vector3f) * world.tile_count, NULL, true);

    FILE *shader_file = fopen("res/shader.glsl", "rb");
    char *shader_source = read_entire_file(shader_file);
    fclose(shader_file);
    shader_create(&shader, shader_source);
    stack_pop_alloc(&temp_mem);
    shader_bind(&shader);
    vertex_attribute_t attributes[] = 
    {
        {
            .name = "vertex_position",
            .binding = 0,
            .size = 3,
            .type = GL_FLOAT,
            .normalized = GL_FALSE
        },
        {
            .name = "vertex_normal",
            .binding = 0,
            .size = 3,
            .type = GL_FLOAT,
            .normalized = GL_FALSE
        },
        {
            .name = "instance_position",
            .binding = 1,
            .size = 3,
            .type = GL_FLOAT,
            .normalized = GL_FALSE
        }
    };
    shader_get_attribute_indices(&shader, attributes, array_length(attributes));

    view_index = glGetUniformLocation(shader.p_handle, "view");
    proj_index = glGetUniformLocation(shader.p_handle, "proj");
    color_index = glGetUniformLocation(shader.p_handle, "color");
    time_index = glGetUniformLocation(shader.p_handle, "time");
    base_index_index = glGetUniformLocation(shader.p_handle, "base_index");
    selected_instance_index = glGetUniformLocation(shader.p_handle, "selected_instance");

    camera_create(&camera, v3f(0.0f, 20.0f, 0.0f), 0.0f, 0.0f, config.fov, (f32) window.size.width / (f32) window.size.height);
    glUniformMatrix4fv(view_index, 1, GL_FALSE, camera.view);
    glUniformMatrix4fv(proj_index, 1, GL_FALSE, camera.proj);

    vao_create(&vao, attributes, array_length(attributes));

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    cursor_locked = true;
    glfwSetInputMode(window.handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPos(window.handle, (f64) window.size.width / 2.0, (f64) window.size.height / 2.0);
    
    // world_load_visible_tiles(&world, &instances_vbo, &visible_instances_vbo, &camera);
}

static void _term()
{

}

static void _update()
{
    stack_flush(&temp_mem);

    if (window.keys[GLFW_KEY_ESCAPE] && window.key_last_update[GLFW_KEY_ESCAPE] == window.ticks)
    {
        cursor_locked = !cursor_locked;
        glfwSetInputMode(window.handle, GLFW_CURSOR, cursor_locked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
        glfwSetCursorPos(window.handle, (f64) window.size.width / 2.0, (f64) window.size.height / 2.0);
    }

    if (cursor_locked)
    {
        camera_update(&camera);
        glfwSetCursorPos(window.handle, (f64) window.size.width / 2.0, (f64) window.size.height / 2.0);

        // static vector3f last_dir = v3(0, 1, 0);
        // static vector3f last_pos = v3(0, 0, 0);
        // if (v3f_dot(&last_dir, &camera.forward) < 0.999f || !v3f_cmp(&last_pos, &camera.position))
        // {
        //     world_load_visible_tiles(&world, &instances_vbo, &visible_instances_vbo, &camera);
        //     last_dir = camera.forward;
        //     last_pos = camera.position;
        // }
    }


    glUniformMatrix4fv(view_index, 1, GL_FALSE, camera.view);
    glUniform1f(time_index, window.time);
    glUniform1i(selected_instance_index, window.ticks);
    // glUniform1i(selected_instance_index, world.tile_count);

    const vector4f sky_color = rgba_hex_to_vec4f(0xffcff3ff);

    glClearColor(sky_color.r, sky_color.g, sky_color.b, sky_color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    vector3f colors[] = { v3f(0.286, 0.894, 0.4), v3f(0.894, 0.819, 0.466), v3f(0, 0, 0.8), v3f(0.95, 0.95, 0.95) };
    const mesh_t *meshes[] = { &tile_meshes.grass_tile_mesh, &tile_meshes.default_mesh, &tile_meshes.default_mesh, &tile_meshes.default_mesh };
    vbo_t *vbos[] = {&tile_meshes.grass_tile_vbo, &tile_meshes.default_vbo, &tile_meshes.default_vbo, &tile_meshes.default_vbo };
    ibo_t *ibos[] = {&tile_meshes.grass_tile_ibo, &tile_meshes.default_ibo, &tile_meshes.default_ibo, &tile_meshes.default_ibo };


    vao_bind(&vao);
    vao_bind_buffer(&vao, 1, &instances_vbo, 1);
    for (int i = 0; i < TILE_TYPE_COUNT; i++)
    {
        ibo_bind(ibos[i]);
        vao_bind_buffer(&vao, 0, vbos[i], 0);
        glUniform3fv(color_index, 1, colors[i].arr);
        glUniform1i(base_index_index, world.type_start[i]);
        glDrawElementsInstancedBaseInstance(GL_TRIANGLES, meshes[i]->index_count, GL_UNSIGNED_SHORT, 0, world.type_count[i], world.type_start[i]);
    }
}

int main(int argc, const char **argv)
{
    config = CONFIG_DEFAULTS;

    // read args
    for (int i = 0; i < argc - 1; i++)
    {
        if (!strcmp(argv[i], "-display_size") || !strcmp(argv[i], "-d"))
        {
            sscanf(argv[i + 1], "%dx%d", &config.display_size.width, &config.display_size.height);
        }
        else if (!strcmp(argv[i], "-fov") || !strcmp(argv[i], "-f"))
        {
            f32 fov_deg;
            sscanf(argv[i + 1], "%f", &fov_deg);
            config.fov = rad(fov_deg);
        }
        else if (!strcmp(argv[i], "-radius") || !strcmp(argv[i], "-r"))
        {
            sscanf(argv[i + 1], "%hd", &config.world_radius);
        }
        else if (!strcmp(argv[i], "-seed") || !strcmp(argv[i], "-s"))
        {
            sscanf(argv[i + 1], "%lld", &config.seed);
        }
    }

    window_create(config.display_size, _init, _term, _update);
    window_loop();
    return 0;
}