#include "af_engine.h"
#include "af_memory.h"

#include <GLFW/glfw3.h>

#define AF_MAX_LAYERS 32

struct AFapp_ctx {
    AFapp_layer *layers[AF_MAX_LAYERS];
    uint32_t layer_count;

    GLFWwindow *window;
};

GlobalVariable char ctx_buf[64 * 1024 * 1024];

GlobalVariable AFapp_ctx app_ctx = {};
GlobalVariable AFarena ctx_arena;

AF_INLINE Internal bool afEngineInit(AFgame_config *config);
AF_INLINE Internal void afEngineRunAll();
AF_INLINE Internal void afEngineFini();

AFAPI AFapp_layer *afLayerCreate(AFtype_id type_id, void *user_data) {
    if (afLayerGet(type_id))
    {
        AFERROR("layer already exists: %u", type_id);
        return NULL;
    }

    AFapp_layer *layer = afArenaAlloc(&ctx_arena, sizeof(AFapp_layer));
    if (!layer) { return NULL; }

    layer->layer_id  = type_id;
    layer->ctx       = &app_ctx;
    layer->user_data = user_data;

    return layer;
}

AFAPI bool afLayerAttach(AFapp_layer *layer) {
    if (!layer) { return false; }
    if (afLayerGet(layer->layer_id))
    {
        AFERROR("layer already exists: %u", layer->layer_id);
        return false;
    }

    app_ctx.layers[app_ctx.layer_count++] = layer;
    if (layer->on_init) { layer->on_init(layer); }

    return true;
}

void afLayerDetach(void *user_data) {
}

void afEngineRun(AFgame_config *config) {
    if (!afEngineInit(config))
    {
        AFINFO("failed to initialize engine (afEngineInit)");
        return;
    }

    afEngineRunAll();
    afEngineFini();
}

bool afEngineInit(AFgame_config *config) {
    afArenaInit(&ctx_arena, ctx_buf, sizeof(ctx_arena));
    *app_ctx.layers = afArenaAlloc(&ctx_arena, sizeof(AFapp_layer) * AF_MAX_LAYERS);

    if (!glfwInit())
    {
        return false;
    }

    app_ctx.window = glfwCreateWindow((int) config->width, (int) config->height, config->title, NULL, NULL);
    if (!app_ctx.window)
    {
        return false;
    }

    return true;
}

void afEngineRunAll() {
    while (!glfwWindowShouldClose(app_ctx.window))
    {
        glfwPollEvents();
        for (uint32_t i = 0; i < app_ctx.layer_count; ++i)
        {
            app_ctx.layers[i]->on_update(app_ctx.layers[i]);
        }
    }
}

void afEngineFini() {
    glfwDestroyWindow(app_ctx.window);
    glfwTerminate();
    afArenaFree_all(&ctx_arena);
}
