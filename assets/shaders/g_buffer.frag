#version 450 core

in VS_OUT {
    vec3 position;
    vec3 normal;
    vec2 uv;
};

uniform sampler2D albedo;
uniform sampler2D normalmap;
uniform sampler2D metalic;
uniform sampler2D rougness;
uniform sampler2D ao;
uniform sampler2D emissionmap;

layout(location = 0) out vec4 position_metal;
layout(location = 1) out vec4 normal_roughness;
layout(location = 2) out vec4 albedo_ao;
layout(location = 3) out vec3 emission;

void main() {
    position_metal.rgb = position;
    position_metal.a = texture(metalic, uv).r;
    normal_roughness.rgb = texture(normalmap, uv).rgb;
    normal_roughness.a = texture(rougness, uv).r;
    albedo_ao.rgb = texture(albedo, uv).rgb;
    albedo_ao.a = texture(ao, uv).a;
    emission.rgb = texture(emissionmap, uv).rgb;
}
