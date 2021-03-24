#ifndef _CAMERA_H
#define _CAMERA_H

#include "util.h"
#include "vectors.h"

#define index_matrix(m, r, c) (m)[(r) + (c) * 4]

typedef struct camera_t
{
    f32 view[16];
    f32 proj[16];

    vector3f position;
    f32 pitch, yaw;

    vector3f side, up, forward;
} camera_t;

void camera_create(camera_t *self, vector3f position, f32 pitch, f32 yaw, f32 fov, f32 aspect_ratio);
void camera_update(camera_t *self);
void camera_move(camera_t *self, vector3f position);
void camera_rotate(camera_t *self, f32 pitch, f32 yaw);
void camera_move_and_rotate(camera_t *self, vector3f position, f32 pitch, f32 yaw);

#endif