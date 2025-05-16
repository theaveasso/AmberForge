#pragma once

#include "amber_forge.h"

extern void afGameCreate(AFgame_config* config);

int main(void) {
    AFgame_config config = {};

    afGameCreate(&config);

    afEngineRun(&config);

    return 0;
}