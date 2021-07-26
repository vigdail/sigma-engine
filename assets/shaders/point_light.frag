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

void main() {
    vec3 frag_position = texture(position_metal, vs_uv).rgb;
    vec3 normal = normalize(texture(normal_roughness, vs_uv).rgb);
    vec3 albedo = texture(albedo_ao, vs_uv).rgb;
    float specular_factor = texture(emission, vs_uv).a;

    vec3 lighting  = albedo * 0.1;
    vec3 viewDir  = normalize(camera_position - frag_position);
    // diffuse
    vec3 lightDir = normalize(light_position - frag_position);
    vec3 diffuse = max(dot(normal, lightDir), 0.0) * albedo * light_color;
    // specular
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 16.0);
    vec3 specular = light_color * spec * specular_factor;
    // attenuation
    float distance = length(light_position - frag_position);
    // TODO: Light radius handling
    float attenuation = light_intensity / (1.0 + distance / light_radius);
    diffuse *= attenuation;
    specular *= attenuation;
    lighting += diffuse + specular;

    fragColor = vec4(lighting, 1.0);
}
