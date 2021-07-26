#version 450 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;

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
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vs_out.normal = normalMatrix * normal;
    vs_out.uv = uv;
    gl_Position = projection * view * worldPos;
}
