#version 410 core

layout (location = 0) in vec4 a_quad;
out vec2 v_uv;

void main() {
    gl_Position = vec4(a_quad.xy, 0.0, 1.0);
    v_uv = vec2(a_quad.wz);
}