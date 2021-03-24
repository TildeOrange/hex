#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "mem.h"
#include "world.h"

static inline u32 _calculate_tile_index(s16 x, s16 y, s16 radius)
{
    // R := radius
    // f(x,R) := -0.5x^2 + (2R + 1.5)x + y { 0 <= x <= R }
    // i = f(x, R) + f(R, R) { 0 <= x <= R }
    // equation: i = -0.5x^2 + (2R + 1.5)x + 1.5R(R + 1) + y { 0 <= x <= R }

    f32 R = (f32) radius;
    f32 xf = (x < 0) * -1.0f * (f32) x + (x >= 0) * (f32) x;
    f32 i = -0.5f * xf * xf + (2.0f * R + 1.5f) * xf;
    return (u32) ((x < 0) * -1.0f * i + (x >= 0) * i + 1.5f * R * (R + 1.0f)) + (u32) y;
}

void convert_cubic_to_world(s16 x, s16 y, vector3f *world)
{
    const f32 xf = (f32) x;
    const f32 yf = (f32) y;
    const f32 zf = -(xf + yf);
    // @todo calculate the value of the x, y, and z axis ahead of time
    const vector3f x_axis = v3f(cosf(rad(30.0f)), 0.0f, sinf(rad(30.0f)));
    const vector3f y_axis = v3f(cosf(rad(150.0f)), 0.0f, sinf(rad(150.0f)));
    const vector3f z_axis = v3f(cosf(rad(270.0f)), 0.0f, sinf(rad(270.0f)));
    world->x = xf * x_axis.x + yf * y_axis.x + zf * z_axis.x;
    world->y = xf * x_axis.y + yf * y_axis.y + zf * z_axis.y;
    world->z = xf * x_axis.z + yf * y_axis.z + zf * z_axis.z;
}


// @note equation for # of hexagons in big hex with a radius of r
// n(r) = 3r^2+3r+1 : r e Z and r > 0

// @note only works for r > 0
static u32 _calculate_hexagon_count(u32 r)
{
    return 3 * r * (r + 1) + 1;
}

static int _compare_tiles(const tile_t *a, const tile_t *b)
{
    return (int) a->type - (int) b->type;
}

void world_create(world_t *self, s16 radius)
{
    self->radius = radius;
    self->tile_count = _calculate_hexagon_count(radius);
    self->tiles = perm_alloc(self->tile_count * sizeof(tile_t));
    self->vbo_coords = perm_alloc(self->tile_count * sizeof(vector3f));

    tile_type_t biome_types[32];
    vector2i biome_anchors[array_length(biome_types)];
    for (int i = 0; i < array_length(biome_types); i++)
    {
        biome_anchors[i].x = rand() % (radius * 2) - radius;
        biome_anchors[i].y = rand() % (radius * 2) - radius;
        biome_types[i] = rand() % (TILE_TYPE_COUNT - 2);
    }

    memset(self->type_count, 0, TILE_TYPE_COUNT * sizeof(u32));

    for (int x = -radius; x <= radius; x++)
    {
        for (int y = -radius - (x < 0) * x; y <= radius - (x > 0) * x; y++)
        {
            tile_t *tile = self->tiles + _calculate_tile_index(x, y, radius);

            tile->x = x;
            tile->y = y;
            tile->altitude = 0;

            u16 distance_to_edge = radius - (abs(x) + abs(y) + abs(x + y)) / 2;
            if (distance_to_edge < 15)
                tile->type = TILE_TYPE_WATER;
            else
            {
                u16 closest_biome_distance = UINT16_MAX;
                for (int i = 0; i < array_length(biome_anchors); i++)
                {
                    const vector2i *anchor = biome_anchors + i;
                    u16 distance = ((u16) abs(anchor->x - tile->x) + (u16) abs(anchor->y - tile->y) + (u16) abs(-(anchor->x + anchor->y) + (tile->x + tile->y))) / 2;
                    
                    if (distance < closest_biome_distance)
                    {
                        closest_biome_distance = distance;
                        tile->type = biome_types[i];
                    }
                }

                if (closest_biome_distance < 2)
                {
                    tile->type = TILE_TYPE_SNOW;
                }
            }

            tile->altitude = 0;

            self->type_count[tile->type]++;
        }
    }

    for (int i = 0; i < TILE_TYPE_COUNT; i++)
    {
        self->type_start[i] = 0;
        for (int j = 0; j < i; j++)
           self->type_start[i] += self->type_count[j];
    }

    // @speedup really this could be done with an array of indices but I'd have
    // make my own qsort or find a library and it's not worth the trouble right now.
    tile_t *sorted_tiles = temp_alloc(self->tile_count * sizeof(tile_t));
    memcpy(sorted_tiles, self->tiles, self->tile_count * sizeof(tile_t));
    qsort(sorted_tiles, self->tile_count, sizeof(tile_t), _compare_tiles);

    for (int i = 0; i < self->tile_count; i++)
    {
        const tile_t *tile = sorted_tiles + i;
        vector3f *vbo_coord = self->vbo_coords + i;
        convert_cubic_to_world(tile->x, tile->y, vbo_coord);
        vbo_coord->y = (f32) tile->altitude * 0.25f;
    }

    temp_free(sorted_tiles);
}