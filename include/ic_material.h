#pragma once

#include <glm/glm.hpp>

#include <map>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace IC {
    enum MaterialFlags {
        None = 0,
        Lit = 1 << 0,
        Transparent = 1 << 1
    };

    enum class BindingType {
        Uniform,
        Texture
    };

    enum class ShaderDataType {
        Float32,
        String,
        Vec2,
        Vec3,
        Vec4
    };

    struct ShaderBinding {
        int index;
        std::string name;
        BindingType bindingType;
        ShaderDataType dataType;
    };

    struct ShaderBindingValue {
        ShaderBinding &binding;
        void *value;
        size_t size;
    };

    class MaterialTemplate {
    public:
        MaterialTemplate(){};
        ~MaterialTemplate(){};

        void AddBinding(int bindingIndex, std::string name, BindingType bindingType, ShaderDataType dataType);

        std::map<int, ShaderBinding> &Bindings() { return _bindings; }

        MaterialFlags flags;

        std::string fragShaderData;
        std::string vertShaderData;

    private:
        std::map<int, ShaderBinding> _bindings;
    };

    class MaterialInstance {
    public:
        MaterialInstance(MaterialTemplate &materialTemplate);
        ~MaterialInstance(){};

        std::map<int, std::shared_ptr<ShaderBindingValue>> BindingValues() { return _bindingValues; }
        MaterialTemplate Template() { return _template; }
        void SetBindingValue(int index, void *value, size_t size);

    private:
        MaterialTemplate &_template;
        std::map<int, std::shared_ptr<ShaderBindingValue>> _bindingValues;
    };
} // namespace IC