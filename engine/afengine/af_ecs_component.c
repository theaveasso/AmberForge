#include "af_ecs_component.h"

#include "af_glrenderer.h"

void RenderComponentImport(ecs_world_t *world) {
    ECS_MODULE(world, RenderComponent);
    ECS_COMPONENT_DEFINE(world, AFcamera3d);
    ECS_COMPONENT_DEFINE(world, AFtransform3d);
    ECS_COMPONENT_DEFINE(world, AFprogram_handle);
}
