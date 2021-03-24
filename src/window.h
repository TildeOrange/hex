#ifndef _WINDOW_H
#define _WINDOW_H

#include <GLFW/glfw3.h>

#include "vectors.h"
#include "util.h"

typedef void (*window_func_t)();

typedef struct window_t
{
    GLFWwindow *handle;

    // callbacks
    window_func_t init, term, update;

    // timing
    f64 time, last_time, dt;
    u64 ticks;

    // input
    vector2i size;
    vector2i cursor_pos, cursor_delta;
    b8 keys[GLFW_KEY_LAST];
    u64 key_last_update[GLFW_KEY_LAST];
} window_t;

void window_create(vector2i display_size, window_func_t init, window_func_t term, window_func_t update);
void window_loop();

// global window
extern window_t window;

#endif