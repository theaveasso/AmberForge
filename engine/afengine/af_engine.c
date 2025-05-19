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
