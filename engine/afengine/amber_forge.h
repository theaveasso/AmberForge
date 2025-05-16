#pragma once

#include <stdint.h>

#include "af_core.h"
#include "af_defines.h"

typedef struct AFgame_config AFgame_config;
struct AFgame_config {
    const char *title;
    uint32_t width;
    uint32_t height;
};

AFAPI void afEngineRun(AFgame_config *config);