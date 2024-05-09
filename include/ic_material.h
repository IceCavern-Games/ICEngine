#pragma once

#include <glm/glm.hpp>

#include <map>
#include <memory>
#include <string>
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

    struct ShaderBinding {
        BindingType type;
        void *data;
        size_t size;
    };

    struct MaterialConstants {
        glm::vec4 color;
    };

    struct LitMaterialProperties {
        glm::vec3 specular;
        glm::float32 shininess;
    };

    class Material {
    public:
        Material(){};
        virtual ~Material(){};

        MaterialFlags flags;

        std::string fragShaderData;
        std::string vertShaderData;

        MaterialConstants constants{};

    protected:
        std::vector<std::string> textures;
        std::map<int, std::unique_ptr<ShaderBinding>> bindings;

    private:
    };

    class LitMaterial : public Material {
    public:
        LitMaterial();
        ~LitMaterial(){};

        std::string diffuseTexturePath;
        std::string specularTexturePath;

    protected:
    private:
        const std::string DEFAULT_VERT_SHADER_PATH = "resources/shaders/default_lit_shader.vert.spv";
        const std::string DEFAULT_FRAG_SHADER_PATH = "resources/shaders/default_lit_shader.frag.spv";
    };

    class UnlitMaterial : public Material {
    public:
        UnlitMaterial();
        ~UnlitMaterial(){};

    private:
        const std::string DEFAULT_VERT_SHADER_PATH = "resources/shaders/default_unlit_shader.vert.spv";
        const std::string DEFAULT_FRAG_SHADER_PATH = "resources/shaders/default_unlit_shader.frag.spv";
    };
} // namespace IC