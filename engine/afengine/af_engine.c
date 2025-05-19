#include "af_engine.h"

#include "af_ecs_system.h"
#include "af_glrenderer.h"
#include "af_memory.h"
#include "af_window.h"

#include "af_ecs_component.h"

#include <GLFW/glfw3.h>

#define AF_MAX_LAYERS 32

struct AFapp_ctx {
    AFapp_layer *layers[AF_MAX_LAYERS];
    uint32_t layer_count;

    GLFWwindow *window;
};

GlobalVariable char ctx_buf[64 * 1024 * 1024];

GlobalVariable AFapp_ctx app_ctx = {};
GlobalVariable ecs_world_t *app_world;
GlobalVariable AFarena ctx_arena;

AF_INLINE Internal void afEngineInit(const AFgame_config *config);
AF_INLINE Internal void afEngineRunAll();
AF_INLINE Internal void afEngineFini();

void afEngineRun(const AFgame_config *config) {
    afEngineInit(config);

    afEngineRunAll();
    afEngineFini();
}

void afEngineInit(const AFgame_config *config) {
    afArenaInit(&ctx_arena, ctx_buf, sizeof(ctx_arena));
    app_world       = ecs_init();
    *app_ctx.layers = afArenaAlloc(&ctx_arena, sizeof(AFapp_layer) * AF_MAX_LAYERS);

    app_ctx.window = afWindowInit((int) config->width, (int) config->height, config->title);

    afInputInit(app_ctx.window, &ctx_arena);
    afGLRendererInit(config->width, config->height);

    glfwSwapInterval(1);

    ECS_IMPORT(app_world, GLRendererSystem);
}

void afEngineRunAll() {
    Persistent bool is_running = true;

    AFtransform3d cam_transform;
    cam_transform.rotation[1] = 0;
    const ecs_entity_t camera = ecs_new(app_world);
    ecs_set(app_world, camera, AFcamera3d, {});
    ecs_set(app_world, camera, AFtransform3d, {.translate = {0, 0, 0}, .rotation = {0, 0, 2}, .scale = {1, 1, 1}});
    ecs_set(app_world, camera, AFprogram_handle, {.handle = renderer_ctx->program_forward});

    const ecs_entity_t cube = ecs_new(app_world);

    while (!glfwWindowShouldClose(app_ctx.window))
    {
        glfwPollEvents();
        ecs_progress(app_world, (float) 1 / 60);
        if (afInputKeyPress(app_ctx.window, AF_KEY_CODE_ESCAPE)) { glfwSetWindowShouldClose(app_ctx.window, GLFW_TRUE); }

        glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        afGLPresent();
        glfwSwapBuffers(app_ctx.window);
    }
}

void afEngineFini() {
    afGLRendererFini();
    afWindowFini(app_ctx.window);
    glfwTerminate();
    afArenaFree_all(&ctx_arena);
}
