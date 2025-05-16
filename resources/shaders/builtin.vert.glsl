#version 410 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_uv;

uniform mat4 u_model;
uniform mat4 u_proj;
uniform mat4 u_view;

void main() {
    gl_Position = u_proj * u_view * u_model * vec4(a_position, 1.0f);
}
