#version 450 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

layout(location = 0) out vec2 vs_uv;

void main() {
    vs_uv = uv;
    gl_Position = proj * view * model * vec4(position, 1.0);
}
