#ifndef _WORLD_H
#define _WORLD_H

#include "util.h"
#include "vectors.h"

// Cubic Coordinate System
//
// Explaniation: https://www.redblobgames.com/grids/hexagons/
//
// Example: 
// Note: the third coordinate is the negation of the sum of the other two
//     (-1, 1) ( 1, 1)
// (-1, 0) ( 0, 0) ( 1, 0)
//     (-1,-1) ( 1,-1)

typedef enum tile_type_t
{
    TILE_TYPE_GRASS,
    TILE_TYPE_SAND,
    TILE_TYPE_WATER,
    TILE_TYPE_SNOW,
    TILE_TYPE_COUNT
} tile_type_t;

typedef struct tile_t
{
    // we don't store z because it can be found with z = -x - y = -(x + y)
    s16 x, y; 
    u8 altitude;
    u8 type;
} tile_t;

typedef struct world_t
{
    s32 radius;
    vector3f *vbo_coords; // these are sorted by type

    tile_t *tiles;
    u32 tile_count;

    u32 type_start[TILE_TYPE_COUNT];
    u32 type_count[TILE_TYPE_COUNT];
} world_t;

void world_create(world_t *self, s16 radius);
void convert_cubic_to_world(s16 x, s16 y, vector3f *world);
u32 _calculate_tile_index(s16 x, s16 y, s16 radius);

#endif