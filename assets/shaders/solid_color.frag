#version 450 core

layout(location = 0) in vec2 vs_uv;

uniform vec3 color = vec3(1.0, 0.0, 0.0);

uniform sampler2D diffuse;

out vec4 fragColor;

void main() {
    fragColor = vec4(color, 1.0f) * texture(diffuse, vs_uv);
}
