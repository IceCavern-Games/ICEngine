#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} mvp;

layout(binding = 1) uniform MaterialConstants {
    vec4 color;
} constants;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {
    gl_Position = mvp.proj * mvp.view * mvp.model * vec4(inPosition, 1.0);

    fragColor = vec4(inColor * constants.color.rgb, 1.0);
    fragTexCoord = inTexCoord;
}