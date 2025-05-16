#version 410 core

in vec2 v_uv;
layout (location = 0) out vec4 out_frag;
uniform sampler2D u_color_tex;

void main() {
    out_frag = vec4(v_uv.x, v_uv.y, v_uv.x * v_uv.y, 1.0);
}
