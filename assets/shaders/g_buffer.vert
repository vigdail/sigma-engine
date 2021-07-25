#version 450 core

layout(location = 0) in vec3 position;
// TODO normals
layout(location = 1) in vec2 uv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out VS_OUT {
    vec3 position;
    vec3 normal;
    vec2 uv;
} vs_out;

void main() {
    vec4 worldPos = model * vec4(position, 1.0);
    vs_out.position = worldPos.xyz;
    vs_out.normal = vec3(0.0, 1.0, 0.0);
    vs_out.uv = uv;
    gl_Position = projection * view * worldPos;
}
