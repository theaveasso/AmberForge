#pragma once

#include "amber_forge.h"
#include "af_engine.h"

extern void afGameCreate(AFgame_config* config);

int main(void) {
    AFgame_config config = {};

    afGameCreate(&config);

    afEngineRun(&config);

    return 0;
}