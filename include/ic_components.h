#pragma once

#include "ic_graphics.h"

#include <glm/glm.hpp>
#include <imgui.h>

#include <vector>

namespace IC {
    class Component {
    public:
        Component();
        virtual ~Component();

        virtual void Gui() = 0;

    protected:
    };

    class Transform : public Component {
    public:
        Transform();
        ~Transform();

        glm::vec3 position = glm::vec3(0.0f);
        glm::vec3 rotation = glm::vec3(0.0f);
        glm::vec3 scale = glm::vec3(0.0f);

        void Gui() override;
    };

    class Mesh : public Component {
    public:
        Mesh();
        ~Mesh();

        bool MeshUpdated() { return _meshUpdatedFlag; }
        MaterialInstance *Material() { return _material.get(); }
        std::vector<VertexData> &Vertices() { return _vertices; }
        std::vector<uint32_t> &Indices() { return _indices; }
        uint32_t VertexCount() { return _vertexCount; }
        uint32_t IndexCount() { return _indexCount; }

        void ClearMeshUpdatedFlag() { _meshUpdatedFlag = false; }
        void SetMaterial(std::shared_ptr<MaterialInstance> &material) { _material = material; }

        void Gui() override;

    private:
        void LoadMesh();

        std::shared_ptr<MaterialInstance> _material;
        std::string _filename = "resources/models/cube.obj";

        std::vector<VertexData> _vertices;
        std::vector<uint32_t> _indices;
        uint32_t _vertexCount;
        uint32_t _indexCount;

        bool _meshUpdatedFlag = false;
    };

    class PointLight : public Component {
    public:
        PointLight();
        ~PointLight();

        float Constant() { return _constant; }
        float Linear() { return _linear; }
        float Quadratic() { return _quadratic; }

        void Gui() override;

        glm::vec3 color = glm::vec3(1.0f);
        glm::vec3 ambient = glm::vec3(0.1f);
        glm::vec3 specular = glm::vec3(1.0f);

    private:
        float _constant = 1.0f;
        float _linear = 0.09f;
        float _quadratic = 0.032f;
    };

    class DirectionalLight : public Component {
    public:
        DirectionalLight();
        ~DirectionalLight();

        glm::vec3 direction;
        glm::vec3 color = glm::vec3(1.0f);
        glm::vec3 ambient = glm::vec3(0.1f);
        glm::vec3 specular = glm::vec3(1.0f);

        void Gui() override;
    };
} // namespace IC