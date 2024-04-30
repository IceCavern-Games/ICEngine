#version 450

#define MAX_POINT_LIGHTS 4

struct DirectionalLightData {
    vec3 dir;
    vec3 amb;
    vec3 diff;
    vec3 spec;
};

struct PointLightData {
    vec3 pos;
    float amb;
    vec3 color;
    float padding;
};

layout(binding = 2) uniform SceneLightData {
    DirectionalLightData directional;
    PointLightData[MAX_POINT_LIGHTS] pointLights;
}
lightData;

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec3 fragPos;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

vec3 calcPointLight(PointLightData light, vec3 fragPos, vec3 normal);
vec3 calcDirectionalLight(DirectionalLightData light);

void main() {
    vec3 result;
    result = calcDirectionalLight(lightData.directional);
    for (int i = 0; i < MAX_POINT_LIGHTS; i++) {
        result += calcPointLight(lightData.pointLights[i], fragPos, normal);
    }
    outColor = vec4(result * fragColor.rgb, 1.0f);
}

vec3 calcPointLight(PointLightData light, vec3 fragPos, vec3 normal) {
    vec3 lightDir = normalize(light.pos - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.color;

    vec3 ambientColor = light.color * light.amb;
    return (ambientColor + diffuse);
}

vec3 calcDirectionalLight(DirectionalLightData light) {
    return vec3(0.0, 0.0, 0.0);
}