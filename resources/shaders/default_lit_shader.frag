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

    vec3 amb;
    vec3 diff;
    vec3 spec;

    float cons;
    float lin;
    float quad;
};

layout(binding = 2) uniform SceneLightData {
    DirectionalLightData directional;
    PointLightData[MAX_POINT_LIGHTS] pointLights;
    uint numPointLights;
}
lightData;

layout(push_constant) uniform PushConstants {
    mat4 model;
    mat4 view;
}
mv;

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec3 fragPos;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

vec3 calcPointLight(PointLightData light);
vec3 calcDirectionalLight(DirectionalLightData light);

void main() {
    vec3 result;
    result = calcDirectionalLight(lightData.directional);
    for (int i = 0; i < lightData.numPointLights; i++) {
        result += calcPointLight(lightData.pointLights[i]);
    }
    outColor = vec4(result * fragColor.rgb, 1.0);
}

vec3 calcPointLight(PointLightData light) {
    // vectors
    vec3 lightPos = vec3(mv.view * vec4(light.pos, 1.0));
    vec3 lightDir = normalize(lightPos - fragPos);
    vec3 viewDir = normalize(-fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);

    float lightDistance = length(lightPos - fragPos);

    // scalar calculations
    float diff = max(dot(normal, lightDir), 0.0);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    float attenuation = 1.0 / (light.cons + light.lin * lightDistance + light.quad * (lightDistance * lightDistance));

    // final values
    vec3 ambient = light.amb * attenuation;
    vec3 diffuse = diff * light.diff * attenuation;
    vec3 specular = spec * light.spec * attenuation;

    return (ambient + diffuse + specular);
}

vec3 calcDirectionalLight(DirectionalLightData light) {
    // vectors
    vec3 lightDir = normalize(vec3(mv.view * vec4(-light.dir, 1.0)));
    vec3 viewDir = normalize(-fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);

    // scalar calculations
    float diff = max(dot(normal, lightDir), 0.0);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

    // final values
    vec3 ambient = light.amb;
    vec3 diffuse = diff * light.diff;
    vec3 specular = spec * light.spec;

    return (ambient + diffuse + specular);
}