#include "af_engine.h"

#include "af_glrenderer.h"
#include "af_memory.h"
#include "af_window.h"

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

AF_INLINE Internal void afEngineInit(const AFgame_config *config);
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

AFAPI AFapp_layer *afLayerGet(uint32_t layer_id) {
    return app_ctx.layers[layer_id];
}

void afEngineRun(const AFgame_config *config) {
    afEngineInit(config);

    afEngineRunAll();
    afEngineFini();
}

void afEngineInit(const AFgame_config *config) {
    afArenaInit(&ctx_arena, ctx_buf, sizeof(ctx_arena));
    *app_ctx.layers = afArenaAlloc(&ctx_arena, sizeof(AFapp_layer) * AF_MAX_LAYERS);

    app_ctx.window = afWindowInit((int) config->width, (int) config->height, config->title);

    afInputInit(app_ctx.window, &ctx_arena);
    afGLRendererInit(config->width, config->height);

    glfwSwapInterval(1);
}

void afEngineRunAll() {
    while (!glfwWindowShouldClose(app_ctx.window))
    {
        glfwPollEvents();

        if (afInputKeyPress(app_ctx.window, AF_KEY_CODE_ESCAPE)) { glfwSetWindowShouldClose(app_ctx.window, GLFW_TRUE); }

        glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        // afGLNewFrame();
        // afGLEndFrame();
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
