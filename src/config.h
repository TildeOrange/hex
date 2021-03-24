#ifndef _CONFIG_H
#define _CONFIG_H

#include "util.h"
#include "vectors.h"

typedef struct config_t
{
    vector2i display_size;
    f32 fov;
    s16 world_radius;
    u64 seed;
    f64 mouse_sensitivity;
} config_t;

#define CONFIG_DEFAULTS (config_t) {\
    .display_size = v2i(1600, 800),\
    .fov = rad(90.0f),\
    .world_radius = 128,\
    .seed = 48,\
    .mouse_sensitivity = 0.25,\
}

// global config
extern config_t config;

#endif