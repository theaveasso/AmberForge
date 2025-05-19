#pragma once

#include "amber_forge.h"

#include <math.h>

Internal AF_INLINE float afSign(const float a) { return (a > 0) ? 1.0f : -1.0f; }

Internal AF_INLINE float afMoveTowards(const float current, const float target, const float max_delta) {
    if (fabsf(target - current) <= max_delta) { return target; }
    return current + afSign(target - current) * max_delta;
}