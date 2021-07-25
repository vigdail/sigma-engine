#version 450 core

layout(location = 0) in vec2 vs_uv;

out vec4 fragColor;

uniform sampler2D image;

void main() {
    fragColor = texture(image, vs_uv);
}
