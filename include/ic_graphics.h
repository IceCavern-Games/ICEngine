#pragma once

#include "ic_material.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <imgui.h>

#include <stdexcept>
#include <string>
#include <vector>

namespace IC {
    enum class RendererType {
        None = -1,
        Vulkan
    };

    struct VertexData {
        glm::vec3 pos;
        glm::vec3 normal;
        glm::vec3 color;
        glm::vec2 texCoord;

        bool operator==(const VertexData &other) const {
            return pos == other.pos && normal == other.normal && color == other.color && texCoord == other.texCoord;
        }
    };

    struct Mesh {
        std::vector<VertexData> vertices;
        std::vector<uint32_t> indices;
        uint32_t vertexCount;
        uint32_t indexCount;
        glm::vec3 pos;
        glm::vec3 scale;
        glm::vec3 rotation;

        void LoadFromFile(std::string fileName);
    };

    struct Light {};

    // lights
    struct PointLight : Light {
        // todo: move mesh and material to gameobject/gameobject components
        glm::vec3 color;
        glm::vec3 ambient;
        glm::vec3 specular;

        // lighting falloff parameters
        float constant = 1.0f;
        float linear = 0.09f;
        float quadratic = 0.032f;

        Mesh previewMesh;
        MaterialInstance *previewMaterial;

        void ParameterGui() {
            ImGui::Begin("Point Light Parameters");
            ImGui::DragFloat3("Light Position", (float *)&previewMesh.pos, 0.01, FLT_MIN, FLT_MAX, "%.3f", 0);
            ImGui::ColorEdit3("Light Color", (float *)&color);
            ImGui::ColorEdit3("Ambient", (float *)&ambient);
            ImGui::ColorEdit3("Specular", (float *)&specular);
            ImGui::DragFloat("Constant", &constant, 0.01, 0.0, FLT_MAX);
            ImGui::DragFloat("Linear", &linear, 0.01, 0.0, FLT_MAX);
            ImGui::DragFloat("Quadratic", &quadratic, 0.01, 0.0, FLT_MAX);
            ImGui::End();
        }
    };

    struct DirectionalLight : Light {
        glm::vec3 direction;
        glm::vec3 color;
        glm::vec3 ambient;
        glm::vec3 specular;

        void ParameterGui() {
            ImGui::Begin("Directional Light Parameters");
            ImGui::DragFloat3("Light Direction", (float *)&direction, 0.01);
            ImGui::ColorEdit3("Light Color", (float *)&color);
            ImGui::ColorEdit3("Ambient", (float *)&ambient);
            ImGui::ColorEdit3("Specular", (float *)&specular);
            ImGui::End();
        }
    };
} // namespace IC

namespace std {
    // define hash function for vertices
    template <> struct hash<IC::VertexData> {
        size_t operator()(IC::VertexData const &vertex) const {
            return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
                   (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
} // namespace std
