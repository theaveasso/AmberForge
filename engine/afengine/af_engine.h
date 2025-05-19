#pragma once

#include "amber_forge.h"

#include <flecs.h>

typedef struct AFapp_ctx AFapp_ctx;

typedef struct AFapp_layer AFapp_layer;

typedef void (*afOnInitFn)(AFapp_layer *layer);
typedef void (*afOnFiniFn)(AFapp_layer *layer);
typedef void (*afOnUpdateFn)(AFapp_layer *layer);

struct AFapp_layer {
    AFapp_ctx *ctx;
    uint32_t layer_id;

    afOnInitFn on_init;
    afOnFiniFn on_fini;
    afOnUpdateFn on_update;

    void *user_data;
};

extern ecs_world_t *app_world;

AFAPI void afEngineRun(const AFgame_config *config);
