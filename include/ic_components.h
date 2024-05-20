#pragma once

#include "ic_graphics.h"

#include <glm/glm.hpp>
#include <imgui.h>

#include <vector>

namespace IC {
    class GameObjectComponent {
    public:
        GameObjectComponent();
        ~GameObjectComponent();

        virtual void Gui() = 0;

    protected:
    };

    class Transform : public GameObjectComponent {
    public:
        Transform();
        ~Transform();

        glm::vec3 Position() { return _position; }
        glm::vec3 Rotation() { return _rotation; }
        glm::vec3 Scale() { return _scale; }

        void SetPosition(glm::vec3 position) { _position = position; }
        void SetRotation(glm::vec3 rotation) { _rotation = rotation; }
        void SetScale(glm::vec3 scale) { _scale = scale; }

        void Gui() override;

    private:
        glm::vec3 _position = glm::vec3(0.0f);
        glm::vec3 _rotation = glm::vec3(0.0f);
        glm::vec3 _scale = glm::vec3(0.0f);
    };

    class Mesh : public GameObjectComponent {
    public:
        Mesh();
        ~Mesh();

        MaterialInstance *Material() { return _material.get(); }
        std::vector<VertexData> &Vertices() { return _vertices; }
        std::vector<uint32_t> &Indices() { return _indices; }
        uint32_t VertexCount() { return _vertexCount; }
        uint32_t IndexCount() { return _indexCount; }

        void SetMaterial(std::shared_ptr<MaterialInstance> &material) { _material = material; }

        void Gui() override;

    private:
        void LoadMesh();

        std::shared_ptr<MaterialInstance> _material;

        std::vector<VertexData> _vertices;
        std::vector<uint32_t> _indices;
        uint32_t _vertexCount;
        uint32_t _indexCount;

        std::string _filename = "resources/models/cube.obj";
    };

    class PointLight : public GameObjectComponent {
    public:
        PointLight();
        ~PointLight();

        glm::vec3 Color() { return _color; }
        glm::vec3 Ambient() { return _ambient; }
        glm::vec3 Specular() { return _specular; }
        float Constant() { return _constant; }
        float Linear() { return _linear; }
        float Quadratic() { return _quadratic; }

        void SetColor(glm::vec3 color) { _color = color; }
        void SetAmbient(glm::vec3 ambient) { _ambient = ambient; }
        void SetSpecular(glm::vec3 specular) { _specular = specular; }

        void Gui() override;

    private:
        glm::vec3 _color = glm::vec3(1.0f);
        glm::vec3 _ambient = glm::vec3(0.1f);
        glm::vec3 _specular = glm::vec3(1.0f);

        float _constant = 1.0f;
        float _linear = 0.09f;
        float _quadratic = 0.032f;
    };

    class DirectionalLight : public GameObjectComponent {
    public:
        DirectionalLight();
        ~DirectionalLight();

        glm::vec3 Direction() { return _direction; }
        glm::vec3 Color() { return _color; }
        glm::vec3 Ambient() { return _ambient; }
        glm::vec3 Specular() { return _specular; }

        void SetDirection(glm::vec3 direction) { _direction = direction; }
        void SetColor(glm::vec3 color) { _color = color; }
        void SetAmbient(glm::vec3 ambient) { _ambient = ambient; }
        void SetSpecular(glm::vec3 specular) { _specular = specular; }

        void Gui() override;

    private:
        glm::vec3 _direction;
        glm::vec3 _color = glm::vec3(1.0f);
        glm::vec3 _ambient = glm::vec3(0.1f);
        glm::vec3 _specular = glm::vec3(1.0f);
    };
} // namespace IC