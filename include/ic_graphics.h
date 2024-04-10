#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <glm/glm.hpp>

#include <string>
#include <stdexcept>
#include <vector>

namespace IC
{
    enum class MaterialInputType
    {
        Texture,
        Color
    };

    struct MaterialConstants
    {
        glm::vec4 color;
    };

    struct ICMaterial
    {
        std::string vertShaderData;
        std::string fragShaderData;

        MaterialConstants constants;
    };

    struct VertexData
    {
        glm::vec3 pos;
        glm::vec3 color;
        glm::vec2 texCoord;

        bool operator==(const VertexData &other) const
        {
            return pos == other.pos && color == other.color && texCoord == other.texCoord;
        }
    };

    struct ICMesh
    {
        std::vector<VertexData> vertices;
        std::vector<uint32_t> indices;
        uint32_t vertex_count;
        uint32_t index_count;

        void load_from_file(std::string file_name);
    };
}

namespace std
{
    // define hash function for vertices
    template <>
    struct hash<IC::VertexData>
    {
        size_t operator()(IC::VertexData const &vertex) const
        {
            return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
                   (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
}