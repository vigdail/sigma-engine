#version 450 core

layout(location = 0) in vec2 vs_uv;

uniform sampler2D position_metal;
uniform sampler2D normal_roughness;
uniform sampler2D albedo_ao;
uniform sampler2D emission;

uniform vec3 light_position;
uniform vec3 light_color;
uniform float light_radius;
uniform float light_intensity;
uniform vec3 camera_position;

out vec4 fragColor;

#define PI 3.1415926535f

float GGXPartialGeometry(float NV, float roughness2)
{
    return NV / mix(NV, 1.0, roughness2);
}

float GGXDistribution(float NH, float roughness)
{
    float roughness2 = roughness * roughness;
    float alpha2 = roughness2 * roughness2;
    float distr = (NH * NH) * (alpha2 - 1.0f) + 1.0f;
    float distr2 = distr * distr;
    float total_distr = alpha2 / (PI * distr2);
    return total_distr;
}

float GGXSmith(float NV, float NL, float roughness)
{
    float d = roughness * 0.125 + 0.125;
    float roughness2 = roughness * d + d;
    return GGXPartialGeometry(NV, roughness2)* GGXPartialGeometry(NL, roughness2);
}

vec3 fresnelSchlick(vec3 F0, float HV)
{
    vec3 fresnel = F0 + (1.0 - F0) * pow(2.0, (-5.55473 * HV - 6.98316) * HV);
    return fresnel;
}

void GGXCookTorranceSampled(vec3 normal, vec3 light_direction, vec3 view_direction, float roughness, float metallic, vec3 albedo,
out vec3 specular, out vec3 diffuse)
{
    vec3 H = normalize(view_direction + light_direction);
    float LV = dot(light_direction, view_direction);
    float NV = dot(normal, view_direction);
    float NL = dot(normal, light_direction);
    float NH = dot(normal, H);
    float HV = dot(H, view_direction);

    if (NV < 0.0 || NL < 0.0)
    {
        specular = vec3(0.0);
        diffuse = vec3(0.0);
        return;
    }

    vec3 F0 = mix(vec3(0.04), albedo, metallic);

    float G = GGXSmith(NV, NL, roughness);
    float D = GGXDistribution(NH, roughness);
    vec3 F = fresnelSchlick(F0, HV);

    specular = D * F * G / (4.0 * NL * NV);
    specular = clamp(specular, vec3(0.0), vec3(1.0));

    float s = max(LV, 0.0) - NL * NV;
    float t = mix(1.0, max(NL, NV), step(0.0, s));
    float d = 1.0 - metallic;

    float sigma2 = roughness * roughness;
    float A = 1.0 + sigma2 * (d / (sigma2 + 0.13) + 0.5 / (sigma2 + 0.33));
    float B = 0.45 * sigma2 / (sigma2 + 0.09);

    diffuse = albedo * NL * (1.0 - F) * (A + B * s / t) / PI;
}

void main() {
    vec3 frag_position = texture(position_metal, vs_uv).rgb;
    float metallic = texture(position_metal, vs_uv).a;
    vec3 normal = normalize(texture(normal_roughness, vs_uv).rgb);
    float roughness = texture(normal_roughness, vs_uv).a;
    vec3 albedo = texture(albedo_ao, vs_uv).rgb;
    float specular_factor = texture(emission, vs_uv).a;
    float ao = texture(albedo_ao, vs_uv).a;

    vec3 light_direction = normalize(light_position - frag_position);
    vec3 view_direction = normalize(camera_position - frag_position);

    vec3 diffuse_color;
    vec3 specular_color;
    GGXCookTorranceSampled(normal, light_direction, view_direction, roughness, metallic, albedo, specular_color, diffuse_color);
    float ambient_factor = 0.1;
    vec3 ambient_color = diffuse_color * ambient_factor;

    float NL = clamp(dot(normal, light_direction), 0.0, 1.0);

    float light_distance = length(light_position - frag_position);

    float attenuation = clamp(1.0f - pow(light_distance / light_radius, 4.0f), 0.0, 1.0);
    float intensity = attenuation * attenuation;
    intensity = isnan(light_distance) || light_radius < light_distance ? 0.0f : intensity * light_intensity;

    vec3 totalColor = (ambient_color + (diffuse_color + specular_color) * NL) * light_color * intensity;
    totalColor = totalColor * ao;

    fragColor = vec4(totalColor, 1.0f);
}
