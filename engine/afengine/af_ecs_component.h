#pragma once

#include <cglm/vec3.h>
#include <flecs.h>

typedef struct AFprogram_handle AFprogram_handle;
struct AFprogram_handle {
    uint32_t handle;
};

typedef struct AFtransform3d AFtransform3d;
struct AFtransform3d {
    vec3 translate;
    vec3 rotation;
    vec3 scale;
};

typedef struct AFcamera3d AFcamera3d;
struct AFcamera3d {
    float near_plane;
    float far_plane;
    float fov;
    float aspect_ratio;
};

ECS_COMPONENT_DECLARE(AFtransform3d);
ECS_COMPONENT_DECLARE(AFcamera3d);
ECS_COMPONENT_DECLARE(AFprogram_handle);

void RenderComponentImport(ecs_world_t *world);