#include <ic_components.h>

#include "ic_log.h"

#include <imgui_stdlib.h>
#include <tiny_obj_loader.h>

namespace IC {
    Component::Component() {}

    Component::~Component() {}

    Transform::Transform() {}

    Transform::~Transform() {}

    void Transform::Gui() {
        ImGui::SeparatorText("TRANSFORM");
        ImGui::DragFloat3("Position", (float *)&position, 0.01);
        ImGui::DragFloat3("Rotation", (float *)&rotation);
        ImGui::DragFloat3("Scale", (float *)&scale);
    }

    Mesh::Mesh() {
        LoadMesh();
    }

    Mesh::~Mesh() {}

    void Mesh::LoadMesh() {
        _vertices.clear();
        _indices.clear();

        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, _filename.c_str())) {
            IC_CORE_ERROR("Failed to load {0}.", _filename);
        }

        std::unordered_map<VertexData, uint32_t> uniqueVertices{};

        for (const auto &shape : shapes) {
            for (const auto &index : shape.mesh.indices) {
                VertexData vertex{};

                vertex.pos = {attrib.vertices[3 * index.vertex_index + 0], attrib.vertices[3 * index.vertex_index + 1],
                              attrib.vertices[3 * index.vertex_index + 2]};

                vertex.normal = {attrib.normals[3 * index.normal_index + 0], attrib.normals[3 * index.normal_index + 1],
                                 attrib.normals[3 * index.normal_index + 2]};

                vertex.texCoord = {attrib.texcoords[2 * index.texcoord_index + 0],
                                   1.0f - attrib.texcoords[2 * index.texcoord_index + 1]};

                vertex.color = {1.0f, 1.0f, 1.0f};

                if (uniqueVertices.count(vertex) == 0) {
                    uniqueVertices[vertex] = static_cast<uint32_t>(_vertices.size());
                    _vertices.push_back(vertex);
                }

                _indices.push_back(uniqueVertices[vertex]);
            }
        }

        _vertexCount = static_cast<uint32_t>(_vertices.size());
        _indexCount = static_cast<uint32_t>(_indices.size());

        _meshUpdatedFlag = true;
    }

    void Mesh::Gui() {
        ImGui::SeparatorText("MESH");
        ImGui::InputText("File Name", &_filename);
        if (ImGui::Button("Load Mesh")) {
            LoadMesh();
        }
    }

    PointLight::PointLight() {}

    PointLight::~PointLight() {}

    void PointLight::Gui() {
        ImGui::SeparatorText("POINT LIGHT");
        ImGui::ColorEdit3("Light Color", (float *)&color);
        ImGui::ColorEdit3("Ambient", (float *)&ambient);
        ImGui::ColorEdit3("Specular", (float *)&specular);
    }

    DirectionalLight::DirectionalLight() {}

    DirectionalLight::~DirectionalLight() {}

    void DirectionalLight::Gui() {
        ImGui::SeparatorText("DIRECTIONAL LIGHT");
        ImGui::DragFloat3("Light Direction", (float *)&direction, 0.01);
        ImGui::ColorEdit3("Light Color", (float *)&color);
        ImGui::ColorEdit3("Ambient", (float *)&ambient);
        ImGui::ColorEdit3("Specular", (float *)&specular);
    }
} // namespace IC