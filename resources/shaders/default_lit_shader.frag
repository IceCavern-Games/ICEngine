#version 450

layout(binding = 2) uniform LightData {
    vec3 pos;
    float ambient;
    vec3 color;
    float padding;
}
lights;

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec3 fragPos;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    vec3 lightDir = normalize(lights.pos - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lights.color;

    vec3 ambientColor = lights.color * lights.ambient;
    outColor = vec4((ambientColor + diffuse) * fragColor.rgb, 1.0f);
}