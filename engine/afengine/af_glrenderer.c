#include "af_glrenderer.h"
#include "af_memory.h"

GlobalVariable const vec3 AF_WORLD_UP       = {0.0f, 1.0f, 0.0f};
GlobalVariable const vec3 AF_WORLD_DOWN     = {0.0f, -1.0f, 0.0f};
GlobalVariable const vec3 AF_WORLD_LEFT     = {-1.0f, 0.0f, 0.0f};
GlobalVariable const vec3 AF_WORLD_RIGHT    = {1.0f, 0.0f, 0.0f};
GlobalVariable const vec3 AF_WORLD_FORWARD  = {0.0f, 0.0f, -1.0f};
GlobalVariable const vec3 AF_WORLD_BACKWARD = {0.0f, 0.0f, 1.0f};

// clang-format off
#include <glad/gl.h>
#include "GLFW/glfw3.h"
// clang-format on

#include "af_log.h"
#include "amber_forge.h"
#include "cglm/cam.h"
#include "cglm/euler.h"

#define AF_MAX_PROGRAM_COUNT 72
#define AF_MAX_FRAMEBUFFER_COUNT 1024
#define AF_MAX_MESH_COUNT 25000

GlobalVariable AFcamera3d *camera;

GlobalVariable char renderer_buf[64 * 1024 * 1024];

GlobalVariable AFgl_program programs[AF_MAX_PROGRAM_COUNT];
uint32_t program_count;

GlobalVariable AFgl_frame_buffer buffers[AF_MAX_FRAMEBUFFER_COUNT];
uint32_t framebuffer_count;

GlobalVariable AFgl_mesh meshes2d[AF_MAX_MESH_COUNT];
uint32_t mesh2d_count;

GlobalVariable AFgl_mesh meshes3d[AF_MAX_MESH_COUNT];
uint32_t mesh3d_count;

Internal AF_INLINE GLuint afglShaderBuild(const char *src, uint32_t type);
Internal AF_INLINE GLuint afglProgramLink(GLuint vert, GLuint frag);
Internal AF_INLINE void afglProgramSetCamera(const AFgl_program *program, const AFtransform3d *transform);

Internal AF_INLINE void afglColorAttachmentCreate(AFgl_frame_buffer *buffer);
Internal AF_INLINE void afglDepthAttachmentCreate(AFgl_frame_buffer *buffer);

Internal AF_INLINE void afglMeshDraw(const AFgl_mesh *mesh, uint32_t mode);
Internal AF_INLINE AFgl_mesh *afglMeshGet(uint32_t mesh_type, uint32_t mesh_handle);

typedef struct AFgl_renderer_ctx AFgl_renderer_ctx;
struct AFgl_renderer_ctx {
    uint32_t fbo_handle;
    uint32_t program_forward;
    uint32_t program_present;
};

GlobalVariable AFgl_renderer_ctx renderer_ctx = {};
GlobalVariable AFarena renderer_arena;

uint32_t afProgramInit(const char *vert_path, const char *frag_path) {
    char *vert_src = afioReadFile(vert_path, &renderer_arena);
    if (!vert_src) { return -1; }
    const GLuint vert = afglShaderBuild(vert_src, GL_VERTEX_SHADER);
    char *frag_src    = afioReadFile(frag_path, &renderer_arena);
    if (!frag_src) { return -1; }
    const GLuint frag = afglShaderBuild(frag_src, GL_FRAGMENT_SHADER);

    const uint32_t handle = program_count++;
    AFgl_program program  = {};
    program.program_id    = afglProgramLink(vert, frag);
    program.model         = glGetUniformLocation(program.program_id, "u_model");
    program.view          = glGetUniformLocation(program.program_id, "u_view");
    program.proj          = glGetUniformLocation(program.program_id, "u_proj");

    programs[handle] = program;

    return handle;
}

void afProgramFini(const uint32_t program_handle) {
    const AFgl_program *program = &programs[program_handle];
    if (program && program->program_id != 0) { glDeleteProgram(program->program_id); }
}

void afProgramBind(const uint32_t program_handle) {
    const AFgl_program *program = &programs[program_handle];
    if (program && program->program_id != 0) { glUseProgram(program->program_id); }
}

void afProgramUnbind(const uint32_t program_handle) {
    const AFgl_program *program = &programs[program_handle];
    if (program && program->program_id != 0) { glUseProgram(0); }
}

uint32_t afFrameBufferInit(const uint32_t width, const uint32_t height) {
    AFgl_frame_buffer frame = {};
    frame.width             = width;
    frame.height            = height;

    glGenFramebuffers(1, &frame.buffer_id);
    glBindFramebuffer(GL_FRAMEBUFFER, frame.buffer_id);

    afglColorAttachmentCreate(&frame);
    afglDepthAttachmentCreate(&frame);

    const uint32_t attachments[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, attachments);

    const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        switch (status)
        {
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: AFERROR("(afFrameBufferInit) incomplete attachment"); break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: AFERROR("(afFrameBufferInit) missing attachment"); break;
            case GL_FRAMEBUFFER_UNSUPPORTED: AFERROR("(afFrameBufferInit) unsupported FBO format"); break;
            default: AFERROR("(afFrameBufferInit) unknown FBO error: 0x%x", status); break;
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    const uint32_t handle = framebuffer_count++;
    buffers[handle]       = frame;
    return handle;
}

void afFrameBufferFini(const uint32_t buffer_handle) {
    AFgl_frame_buffer *buffer = &buffers[buffer_handle];
    AF_ASSERT(buffer != NULL && "(afglCreateColorAttachment) invalid buffer handle");
    if (!buffer) { return; }
    glDeleteTextures(1, &buffer->color_tex);
    glDeleteRenderbuffers(1, &buffer->depth_tex);
    glDeleteFramebuffers(1, &buffer->buffer_id);
}

void afMeshInit(const uint32_t mesh_type, const uint32_t mesh_handle) {
    AFgl_mesh *mesh = afglMeshGet(mesh_type, mesh_handle);
    if (!mesh) { return; }

    glGenVertexArrays(1, &mesh->data.buffer_id);
    glBindVertexArray(mesh->data.buffer_id);

    uint32_t VBO = 0u;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    if (mesh_type == AFGL_MESH_TYPE_2D)
    {
        glBufferData(GL_ARRAY_BUFFER, mesh->data.vertex_count * sizeof(AFgl_quad), mesh->data.vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(AFgl_quad), (void *) offsetof(AFgl_quad, data));
    }
    else if (mesh_type == AFGL_MESH_TYPE_3D)
    {
        glBufferData(GL_ARRAY_BUFFER, mesh->data.vertex_count * sizeof(AFgl_vertex), mesh->data.vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(AFgl_vertex), (void *) offsetof(AFgl_vertex, position));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(AFgl_vertex), (void *) offsetof(AFgl_vertex, normal));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(AFgl_vertex), (void *) offsetof(AFgl_vertex, uvs));
    }
    else
    {
        AFERROR("(afMeshInit) invalid mesh type");
        return;
    }

    if (mesh->data.index_count != 0u)
    {
        uint32_t EBO = 0u;
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->data.index_count * sizeof(uint32_t), mesh->data.indices, GL_STATIC_DRAW);
    }

    glBindVertexArray(0);
}

void afMeshFini(const uint32_t mesh_type, const uint32_t mesh_handle) {
    const AFgl_mesh *mesh = afglMeshGet(mesh_type, mesh_handle);
    if (mesh && mesh->data.buffer_id != 0) { glDeleteVertexArrays(1, &mesh->data.buffer_id); }
}

void afMesh2dDraw(uint32_t mesh_handle, const uint32_t mode) {
    AFgl_mesh *mesh = &meshes2d[mesh_handle];
    if (!mesh) { return; }
    afglMeshDraw(mesh, mode);
}

void afMesh3dDraw(const uint32_t mesh_handle, const uint32_t mode) {
    AFgl_mesh *mesh = &meshes3d[mesh_handle];
    if (!mesh) { return; }
    afglMeshDraw(mesh, mode);
}

void afGLRendererInit(const uint32_t width, const uint32_t height) {
    afArenaInit(&renderer_arena, renderer_buf, sizeof(renderer_buf));

    gladLoadGL(glfwGetProcAddress);

    renderer_ctx.fbo_handle      = afFrameBufferInit(width, height);
    renderer_ctx.program_forward = afProgramInit("resources/shaders/builtin.vert.glsl", "resources/shaders/builtin.frag.glsl");

    renderer_ctx.program_present = afProgramInit("resources/shaders/builtin.present.vert.glsl", "resources/shaders/builtin.present.frag.glsl");
    AFgl_program *program        = &programs[renderer_ctx.program_present];
    program->mesh_id             = afQuadMesh2dCreate();

    afCameraInit((float) width / (float) height);
}

void afGLRendererFini() {
    afFrameBufferFini(renderer_ctx.fbo_handle);
}

void afGLRendererSetCamera(const uint32_t program_handle, AFtransform3d *transform) {
    AFgl_program *program = &(programs[program_handle]);
}

void afGLNewFrame() {
    AFgl_frame_buffer *buffer = &buffers[renderer_ctx.fbo_handle];
    if (!buffer) { return; }
    glBindFramebuffer(GL_FRAMEBUFFER, buffer->buffer_id);
    glViewport(0, 0, (int) buffer->width, (int) buffer->height);
    glClearColor(0, 0, 0, 1);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
}

void afGLEndFrame() {
    glDisable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

AFAPI void afGLPresent() {
    AFgl_program *program = &programs[renderer_ctx.program_present];
    if (!program) { return; }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, 1280, 720);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(program->program_id);
    afMesh2dDraw(program->mesh_id, GL_TRIANGLES);
    glUseProgram(0);
}

GLuint afglShaderBuild(const char *src, const uint32_t type) {
    const GLuint shader_id = glCreateShader(type);
    glShaderSource(shader_id, 1, &src, NULL);
    glCompileShader(shader_id);

    char *error    = afArenaAlloc(&renderer_arena, 512);
    int32_t status = 0;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);
    if (!status)
    {
        glGetShaderInfoLog(shader_id, sizeof(error), NULL, error);
        glDeleteShader(shader_id);
        AFERROR("(afglShaderBuild) failed to build shader from %s error: %s", src, error);
        abort();
    }
    return shader_id;
}

GLuint afglProgramLink(const GLuint vert, const GLuint frag) {
    const GLuint program_id = glCreateProgram();
    glAttachShader(program_id, vert);
    glAttachShader(program_id, frag);
    glLinkProgram(program_id);

    char *error    = afArenaAlloc(&renderer_arena, 512);
    int32_t status = 0;
    glGetProgramiv(program_id, GL_LINK_STATUS, &status);
    if (!status)
    {
        glGetProgramInfoLog(program_id, 512, NULL, error);
        glDeleteProgram(program_id);
        AFERROR("(afglProgramLink) failed to link program error: %s", error);
        abort();
    }
    glDeleteShader(vert);
    glDeleteShader(frag);
    return program_id;
}

Internal AF_INLINE void afglProgramSetCamera(const AFgl_program *program, const AFtransform3d *transform) {
    mat4 view, proj;
    afCameraView(transform, view);
    afCameraProj(proj);

    glUniformMatrix4fv((GLint) program->proj, 1, GL_FALSE, (const float *) proj);
    glUniformMatrix4fv((GLint) program->view, 1, GL_FALSE, (const float *) view);
}

void afglColorAttachmentCreate(AFgl_frame_buffer *buffer) {
    glGenTextures(1, &buffer->color_tex);
    glBindTexture(GL_TEXTURE_2D, buffer->color_tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, (int32_t) buffer->width, (int32_t) buffer->height, 0, GL_RGBA, GL_FLOAT, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer->color_tex, 0);
}

void afglDepthAttachmentCreate(AFgl_frame_buffer *buffer) {
    glGenTextures(1, &buffer->depth_tex);
    glBindTexture(GL_TEXTURE_2D, buffer->depth_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, (int32_t) buffer->width, (int32_t) buffer->height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, buffer->depth_tex, 0);
}

void afglMeshDraw(const AFgl_mesh *mesh, const uint32_t mode) {
    glBindVertexArray(mesh->data.buffer_id);
    if (mesh->data.index_count != 0u)
    {
        glDrawElements(mode, (GLint) mesh->data.index_count, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        return;
    }
    glDrawArrays(mode, 0, (GLint) mesh->data.vertex_count);
    glBindVertexArray(0);
}

AFgl_mesh *afglMeshGet(const uint32_t mesh_type, const uint32_t mesh_handle) {
    switch (mesh_type)
    {
        case AFGL_MESH_TYPE_2D: return &meshes2d[mesh_handle];
        case AFGL_MESH_TYPE_3D: return &meshes3d[mesh_handle];
        default: break;
    }
    return NULL;
}

uint32_t afQuadMesh2dCreate() {
    AFgl_quad *vertices = afArenaAlloc(&renderer_arena, sizeof(AFgl_quad) * 4);
    uint32_t *indices   = afArenaAlloc(&renderer_arena, sizeof(uint32_t) * 6);

    vertices[0] = (AFgl_quad) {-1.0f, -1.0f, 0.0f, 0.0f};
    vertices[1] = (AFgl_quad) {-1.0f, 1.0f, 0.0f, 1.0f};
    vertices[2] = (AFgl_quad) {1.0f, 1.0f, 0.0f, 1.0f};
    vertices[3] = (AFgl_quad) {1.0f, -1.0f, 0.0f, 0.0f};

    // clang-format off
    indices[0] = 0; indices[1] = 1; indices[2] = 2;
    indices[3] = 0; indices[4] = 2; indices[5] = 3;
    // clang-format on

    AFgl_mesh mesh = {};
    mesh.type      = AFGL_MESH_TYPE_2D;
    mesh.data      = (AFgl_mesh_data) {
             .vertices      = vertices,
             .vertex_count  = 4,
             .vertex_stride = sizeof(AFgl_quad),
             .indices       = indices,
             .index_count   = 6};

    const uint32_t handle = mesh2d_count++;
    meshes2d[handle]      = mesh;
    afMeshInit(AFGL_MESH_TYPE_2D, handle);
    return handle;
}

uint32_t afQuadMesh3dCreate() {
    AFgl_vertex *vertices = afArenaAlloc(&renderer_arena, sizeof(AFgl_vertex) * 4);
    uint32_t *indices     = afArenaAlloc(&renderer_arena, sizeof(uint32_t) * 6);

    vertices[0] = (AFgl_vertex) {{-0.5f, -0.5f, 0.0f}, {0, 0, 1}, {0.0f, 0.0f}};
    vertices[1] = (AFgl_vertex) {{0.5f, -0.5f, 0.0f}, {0, 0, 1}, {1.0f, 0.0f}};
    vertices[2] = (AFgl_vertex) {{0.5f, 0.5f, 0.0f}, {0, 0, 1}, {1.0f, 1.0f}};
    vertices[3] = (AFgl_vertex) {{-0.5f, 0.5f, 0.0f}, {0, 0, 1}, {0.0f, 1.0f}};

    // clang-format off
    indices[0] = 0; indices[1] = 1; indices[2] = 2;
    indices[3] = 0; indices[4] = 2; indices[5] = 3;
    // clang-format on

    AFgl_mesh mesh = {};
    mesh.type      = AFGL_MESH_TYPE_3D;
    mesh.data      = (AFgl_mesh_data) {
             .vertices      = vertices,
             .vertex_count  = 4,
             .vertex_stride = sizeof(AFgl_quad),
             .indices       = indices,
             .index_count   = 6};

    const uint32_t handle = mesh3d_count++;
    meshes3d[handle]      = mesh;
    return handle;
}

void afCameraInit(const float aspect_ratio) {
    camera               = afArenaAlloc(&renderer_arena, sizeof(AFcamera3d));
    camera->fov          = 45.0f;
    camera->aspect_ratio = aspect_ratio;
    camera->near_plane   = 0.1f;
    camera->far_plane    = 1000.0f;
}

void afCameraFrustum(const AFtransform3d *transform, mat4 out) {
    mat4 view, proj;
    afCameraView(transform, view);
    afCameraProj(proj);
    return glm_mat4_mul(proj, view, out);
}

void afCameraView(const AFtransform3d *transform, mat4 out) {
    mat4 rotation;

    vec3 target = {0.0f, 1.0f, 0.0f};
    vec3 rot_rad;
    glm_vec3_copy(transform->rotation, rot_rad);
    glm_vec3_scale(rot_rad, GLM_PI / 180.0f, rot_rad);
    glm_euler_xyz(rot_rad, rotation);

    glm_mat4_mulv3(rotation, AF_WORLD_FORWARD, 1.0f, target);
    glm_vec3_add(transform->translate, target, target);

    glm_lookat(transform->translate, target, AF_WORLD_UP, out);
}

void afCameraProj(mat4 out) {
    glm_perspective(camera->fov, camera->aspect_ratio, camera->near_plane, camera->far_plane, out);
}
