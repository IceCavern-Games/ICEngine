#version 450

layout(binding = 2) uniform LightData {
    vec3 pos;
    float ambient;
    vec3 color;
    float padding;
}
lights;

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    vec3 ambientColor = lights.color * lights.ambient;
    outColor = vec4(fragColor.rgb * ambientColor, 1.0f);
}