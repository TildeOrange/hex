#include <string.h> // memset
#include <math.h> 
#include "config.h"
#include "window.h"
#include "camera.h"

static void _update_view_pos(camera_t *self)
{
    index_matrix(self->view, 0, 3) = -v3f_dot(&self->position, &self->side);
    index_matrix(self->view, 1, 3) = -v3f_dot(&self->position, &self->up);
    index_matrix(self->view, 2, 3) = v3f_dot(&self->position, &self->forward);
}

static void _update_view(camera_t *self)
{
    const f32 yaw_rad = rad(self->yaw - 90.0f);
    const f32 pitch_rad = rad(self->pitch + 90.0f);
    const f32 pitch_sin = sinf(pitch_rad);

    self->up = v3f(0, 1, 0);
    self->forward = v3f(pitch_sin * cosf(yaw_rad), cosf(pitch_rad), pitch_sin * sinf(yaw_rad));
    v3f_norm(&self->forward, &self->forward);
    v3f_cross(&self->forward, &self->up, &self->side);
    v3f_norm(&self->side, &self->side);
    v3f_cross(&self->side, &self->forward, &self->up);

    index_matrix(self->view, 0, 0) = self->side.x;
    index_matrix(self->view, 0, 1) = self->side.y;
    index_matrix(self->view, 0, 2) = self->side.z;
    index_matrix(self->view, 0, 3) = -v3f_dot(&self->position, &self->side);
    index_matrix(self->view, 1, 0) = self->up.x;
    index_matrix(self->view, 1, 1) = self->up.y;
    index_matrix(self->view, 1, 2) = self->up.z;
    index_matrix(self->view, 1, 3) = -v3f_dot(&self->position, &self->up);
    index_matrix(self->view, 2, 0) = -self->forward.x;
    index_matrix(self->view, 2, 1) = -self->forward.y;
    index_matrix(self->view, 2, 2) = -self->forward.z;
    index_matrix(self->view, 2, 3) = v3f_dot(&self->position, &self->forward);
    index_matrix(self->view, 3, 0) = 0.0f;
    index_matrix(self->view, 3, 1) = 0.0f;
    index_matrix(self->view, 3, 2) = 0.0f;
    index_matrix(self->view, 3, 3) = 1.0f;

    // printf("side vector = (%.3f, %.3f, %.3f)\n", self->side.x, self->side.y, self->side.z);
    // printf("forward vector = (%.3f, %.3f, %.3f)\n", self->forward.x, self->forward.y, self->forward.z);
    // printf("u = (%.3f, %.3f, %.3f)\n", u.x, u.y, u.z);

    // printf("\n");
    // for (int r = 0; r < 4; r++)
    // {
    //     for (int c = 0; c < 4; c++)
    //     {
    //         printf("%4.4f, ", index_matrix(self->view, r, c));
    //     }
    //     printf("\n");
    // }
}

static void _update_proj(camera_t *self, f32 fov, f32 aspect_ratio)
{
    const f32 near_val = 0.01f, far_val = 1000.0f;
    const f32 f = 1.0f / tanf(fov * 0.5f), fn = 1.0f / (near_val - far_val);

    memset(self->proj, 0, sizeof(f32) * 16);
    index_matrix(self->proj, 0, 0) = f / aspect_ratio;
    index_matrix(self->proj, 1, 1) = f;
    index_matrix(self->proj, 2, 2) = (near_val + far_val) * fn;
    index_matrix(self->proj, 3, 2) = -1.0f;
    index_matrix(self->proj, 2, 3) = 2.0f * near_val * far_val * fn;
}

void camera_create(camera_t *self, vector3f position, f32 pitch, f32 yaw, f32 fov, f32 aspect_ratio)
{
    camera_move_and_rotate(self, position, pitch, yaw);
    _update_proj(self, fov, aspect_ratio);
}

void camera_update(camera_t *self)
{
    if (window.keys[GLFW_KEY_LEFT_SHIFT] && window.keys[GLFW_KEY_R])
    {
        self->position = v3f(0, 1, 0);
        self->pitch = 0.0f;
        self->yaw = 0.0f;
    }
    else
    {
        vector3f delta_pos = v3f(0, 0, 0);

        if (window.keys[GLFW_KEY_UP])
            self->pitch -= 90.0f * window.dt;

        if (window.keys[GLFW_KEY_DOWN])
            self->pitch += 90.0f * window.dt;

        if (window.keys[GLFW_KEY_LEFT])
            self->yaw -= 90.0f * window.dt;

        if (window.keys[GLFW_KEY_RIGHT])
            self->yaw += 90.0f * window.dt;

        f64 cx, cy;
        glfwGetCursorPos(window.handle, &cx, &cy);
        cx -= (f64) window.size.width / 2.0;  
        cy -= (f64) window.size.height / 2.0;

        self->yaw += cx * config.mouse_sensitivity;
        self->pitch += cy * config.mouse_sensitivity;


        if (self->yaw > 360.0f)
            self->yaw -= 360.0f;

        self->pitch = clamp(self->pitch, -89.9f, 89.9f);

        f32 delta_forward = 0.0f, delta_side = 0.0f;

        if (window.keys[GLFW_KEY_W])
            delta_forward += 1.0f;

        if (window.keys[GLFW_KEY_S])
            delta_forward -= 1.0f;

        if (window.keys[GLFW_KEY_A])
            delta_side -= 1.0f;

        if (window.keys[GLFW_KEY_D])
            delta_side += 1.0f;

        if (window.keys[GLFW_KEY_SPACE] || window.keys[GLFW_KEY_E])
            delta_pos.y += 1.0f;

        if (window.keys[GLFW_KEY_LEFT_CONTROL] || window.keys[GLFW_KEY_C])
            delta_pos.y -= 1.0f;

        delta_pos.x = delta_forward * self->forward.x + delta_side * self->side.x;
        delta_pos.z = delta_forward * self->forward.z + delta_side * self->side.z;

        const f32 speed = window.keys[GLFW_KEY_LEFT_SHIFT] ? 100.0f : 30.0f;
        const f32 mag = v3f_mag(&delta_pos);
        if (mag > 0.0f)
        {
            v3f_scale(speed * window.dt / mag, &delta_pos, &delta_pos);
            v3f_add(&delta_pos, &self->position, &self->position);
        }

        if (self->position.y < 1.0) self->position.y = 1.0f;
    }

    _update_view(self);
}

void camera_move(camera_t *self, vector3f position)
{
    self->position = position;
    _update_view(self);
}

void camera_rotate(camera_t *self, f32 pitch, f32 yaw)
{
    self->pitch = pitch;      
    self->yaw = yaw;
    _update_view(self);
}

void camera_move_and_rotate(camera_t *self, vector3f position, f32 pitch, f32 yaw)
{
    self->position = position;
    self->pitch = pitch;      
    self->yaw = yaw;
    _update_view(self);
}