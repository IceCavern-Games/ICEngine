#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#include <stdexcept>
#include <string>
#include <vector>

namespace IC {
    enum class RendererType {
        None = -1,
        Vulkan
    };

    enum class MaterialInputType {
        Texture,
        Color
    };

    struct MaterialConstants {
        glm::vec4 color;
    };

    struct Material {
        std::string fragShaderData;
        std::string vertShaderData;

        MaterialConstants constants;
    };

    struct VertexData {
        glm::vec3 pos;
        glm::vec3 color;
        glm::vec2 texCoord;

        bool operator==(const VertexData &other) const {
            return pos == other.pos && color == other.color && texCoord == other.texCoord;
        }
    };

    struct Mesh {
        std::vector<VertexData> vertices;
        std::vector<uint32_t> indices;
        uint32_t vertexCount;
        uint32_t indexCount;

        void LoadFromFile(std::string fileName);
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
