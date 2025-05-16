#pragma once

#include "amber_forge.h"
#include <glad/gl.h>

typedef struct AFgl_frame_buffer AFgl_frame_buffer;
typedef struct AFgl_shader AFgl_shader;
typedef struct AFgl_pbr_shader AFgl_pbr_shader;

struct AFgl_frame_buffer {
    int32_t width;
    int32_t height;
    uint32_t render;
    uint32_t color;
    uint32_t buffer_id;
};

uint32_t afFrameBufferInit(int32_t width, int32_t height);
void afFrameBufferFini(uint32_t buffer_handle);

void afRendererInit();
void afRendererFini();

void afNewFrame();
void afEndFrame();