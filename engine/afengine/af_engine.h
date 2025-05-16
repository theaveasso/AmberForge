#pragma once

#include "af_log.h"
#include "amber_forge.h"

typedef uint32_t AFtype_id;

#define AF_TYPEID_INVALID 0

#define AF_TYPEID_DEFINE(NAME, ID) \
    enum                           \
    {                              \
        TYPE_ID_##NAME = ID        \
    }

AF_TYPEID_DEFINE(EDITOR, 1);

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

AFAPI AFapp_layer *afLayerCreate(AFtype_id type_id, void *user_data);
AFAPI bool afLayerAttach(AFapp_layer *layer);
AFAPI void afLayerDetach(void *user_data);
AFAPI AFapp_layer *afLayerGet(uint32_t layer_id);

AFAPI void afEngineRun(const AFgame_config *config);
