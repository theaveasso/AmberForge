#include "af_ecs_system.h"
#include "af_ecs_component.h"
#include "af_glrenderer.h"
#include "amber_forge.h"

Internal void GLSetProgramCamera(ecs_iter_t *it) {
    AFtransform3d *transform        = ecs_field(it, AFtransform3d, 1);
    const AFprogram_handle *program = ecs_field(it, AFprogram_handle, 2);

    for (int32_t i = 0; i < it->count; ++i)
    {
        afGLRendererSetCamera(program->handle, transform);
    }
}

void GLRendererSystemImport(ecs_world_t *world) {
    ECS_MODULE(world, GLRendererSystem);
    ECS_IMPORT(world, RenderComponent);

    ECS_SYSTEM(world, GLSetProgramCamera, EcsPostLoad, [inout] render.component.AFcamera3d, [in] render.component.AFtransform3d, [in] render.component.AFprogram_handle);
}
