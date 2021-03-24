#include "window.h"

// global window
window_t window;

// forward decl event handlers
static void _handle_key(GLFWwindow *handle, int code, int scancode, int action, int mods);
static void _handle_cursor_pos(GLFWwindow *handle, double x, double y);
static void _handle_cursor_enter(GLFWwindow *handle, int entered);

void window_create(vector2i display_size, window_func_t init, window_func_t term, window_func_t update)
{
    if (!glfwInit())
    {
        log_fatal_error("Failed to initalize GLFW");
    }

    window.init = init;
    window.term = term;
    window.update = update;

    window.time = 0.0;
    window.dt = 0.0;
    window.ticks = 0;

    window.size = display_size;
    memset(window.keys, sizeof(b8) * GLFW_KEY_LAST);

    glfwWindowHint(GLFW_SAMPLES, 4);

    window.handle = glfwCreateWindow(display_size.width, display_size.height, "hekss", NULL, NULL);
    glfwSetKeyCallback(window.handle, _handle_key);
    glfwSetCursorPosCallback(window.handle, _handle_cursor_pos);
    glfwSetCursorEnterCallback(window.handle, _handle_cursor_enter);

    glfwMakeContextCurrent(window.handle);
    if (glewInit() != GL_NO_ERROR)
    {
        log_fatal_error("Failed to initalize GLEW");
    }
}

void window_loop()
{
    window.init();

    window.last_time = glfwGetTime();
    while (!glfwWindowShouldClose(window.handle))
    {
        window.time = glfwGetTime();
        window.dt = window.time - window.last_time;
        window.last_time = window.time;
        window.cursor_pos = v2i(0, 0);
        glfwPollEvents();
        window.update();
        glfwSwapInterval(1);
        glfwSwapBuffers(window.handle);
        window.ticks++;
    }

    window.term();
}

// event handlers

static void _handle_key(GLFWwindow *handle, int code, int scancode, int action, int mods)
{
    window.keys[code] = (action == GLFW_REPEAT || action == GLFW_PRESS);
    if (action == GLFW_PRESS || action == GLFW_RELEASE)
        window.key_last_update[code] = window.ticks;
}

static void _handle_cursor_pos(GLFWwindow *handle, double x, double y)
{
    vector2i pos = v2i(x, y);
    v2i_sub(&pos, &window.cursor_pos, &window.cursor_delta);
    window.cursor_pos = pos;
}

static void _handle_cursor_enter(GLFWwindow *handle, int entered)
{
    double x, y;
    glfwGetCursorPos(window.handle, &x, &y);
    window.cursor_pos = v2i(x, y);
}