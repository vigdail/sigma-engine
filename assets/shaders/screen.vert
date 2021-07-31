#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;

layout(location = 0) out vec2 vs_uv;

void main() {
    vs_uv = uv;
    gl_Position = vec4(position * 2.0f - vec3(1.0, 1.0, 0.0), 1.0);
}
