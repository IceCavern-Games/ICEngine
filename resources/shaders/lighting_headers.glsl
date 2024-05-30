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

layout(set = 1, binding = 0) uniform SceneLightData {
    DirectionalLightData directional;
    PointLightData[MAX_POINT_LIGHTS] pointLights;
    uint numPointLights;
}
lightData;
layout(set = 1, binding = 1) uniform sampler2D shadowMap;

layout(set = 2, binding = 1) uniform sampler2D diffuseTexture;
layout(set = 2, binding = 2) uniform sampler2D specularMask;

vec3 calcPointLight(PointLightData light, vec2 texCoord, vec3 normal, vec3 fragPos) {
    // vectors
    vec3 lightDir = normalize(light.pos - fragPos);
    vec3 viewDir = normalize(-fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);

    float lightDistance = length(light.pos - fragPos);

    // scalar calculations
    float diff = max(dot(normal, lightDir), 0.0);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    float attenuation = 1.0 / (light.cons + light.lin * lightDistance + light.quad * (lightDistance * lightDistance));

    // final values
    vec3 ambient = light.amb * attenuation * vec3(texture(diffuseTexture, texCoord));
    vec3 diffuse = diff * light.diff * attenuation * vec3(texture(diffuseTexture, texCoord));
    vec3 specular = spec * light.spec * attenuation * vec3(texture(specularMask, texCoord));

    return (ambient + diffuse + specular);
}

vec3 calcDirectionalLight(DirectionalLightData light, vec2 texCoord, vec3 normal, vec3 fragPos) {
    // vectors
    vec3 lightDir = normalize(-light.dir);
    vec3 viewDir = normalize(-fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);

    // scalar calculations
    float diff = max(dot(normal, lightDir), 0.0);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

    // final values
    vec3 ambient = light.amb * vec3(texture(diffuseTexture, texCoord));
    vec3 diffuse = diff * light.diff * vec3(texture(diffuseTexture, texCoord));
    vec3 specular = spec * light.spec * vec3(texture(specularMask, texCoord));

    return (ambient + diffuse + specular);
}