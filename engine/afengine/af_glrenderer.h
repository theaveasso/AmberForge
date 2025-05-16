#pragma once

#include "amber_forge.h"

#include <cglm/vec3.h>
#include <glad/gl.h>

typedef struct AFgl_program AFgl_program;
typedef struct AFgl_frame_buffer AFgl_frame_buffer;

typedef struct AFgl_quad AFgl_quad;
typedef struct AFgl_flat_vertex AFgl_flat_vertex;
typedef struct AFgl_vertex AFgl_vertex;
typedef struct AFgl_mesh_data AFgl_mesh_data;
typedef struct AFgl_mesh AFgl_mesh;

typedef enum
{
    AFGL_MESH_TYPE_2D,
    AFGL_MESH_TYPE_3D,
    AFGL_MESH_TYPE_MAX_COUNT,
} AFgl_mesh_type;

struct AFgl_quad {
    float data[4];
};

struct AFgl_vertex {
    vec3 position;
    vec3 normal;
    vec2 uvs;
};

struct AFgl_mesh_data {
    void *vertices;
    uint32_t vertex_count;
    uint32_t vertex_stride;

    uint32_t *indices;
    uint32_t index_count;

    uint32_t buffer_id;
};

struct AFgl_mesh {
    AFgl_mesh_type type;
    AFgl_mesh_data data;
};

struct AFgl_frame_buffer {
    GLuint buffer_id;

    uint32_t width;
    uint32_t height;

    uint32_t depth_tex;
    uint32_t color_tex;
};

struct AFgl_program {
    uint32_t program_id;
    uint32_t mesh_id;

    uint32_t model;
    uint32_t view;
    uint32_t proj;
};

AFAPI uint32_t afProgramInit(const char *vert_path, const char *frag_path);
AFAPI void afProgramFini(uint32_t program_handle);
AFAPI void afProgramBind(uint32_t program_handle);
AFAPI void afProgramUnbind(uint32_t program_handle);

uint32_t afFrameBufferInit(uint32_t width, uint32_t height);
void afFrameBufferFini(uint32_t buffer_handle);

void afMeshInit(uint32_t mesh_type, uint32_t mesh_handle);
void afMeshFini(uint32_t mesh_type, uint32_t mesh_handle);
void afMesh2dDraw(uint32_t mesh_handle, uint32_t mode);
void afMesh3dDraw(uint32_t mesh_handle, uint32_t mode);

void afGLRendererInit(uint32_t width, uint32_t height);
void afGLRendererFini();

AFAPI void afGLNewFrame();
AFAPI void afGLEndFrame();
AFAPI void afGLPresent();

AFAPI uint32_t afQuadMesh2dCreate();
AFAPI uint32_t afQuadMesh3dCreate();