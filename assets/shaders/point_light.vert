#version 450 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

layout(location = 0) out vec2 vs_uv;

void main() {
    vs_uv = uv;
    gl_Position = vec4(position * 2.0 - vec3(1.0f), 1.0);
}
