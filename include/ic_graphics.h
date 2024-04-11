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
        glm::vec4 Color;
    };

    struct Material
    {
        std::string FragShaderData;
        std::string VertShaderData;

        MaterialConstants Constants;
    };

    struct VertexData
    {
        glm::vec3 Pos;
        glm::vec3 Color;
        glm::vec2 TexCoord;

        bool operator==(const VertexData &other) const
        {
            return Pos == other.Pos && Color == other.Color && TexCoord == other.TexCoord;
        }
    };

    struct Mesh
    {
        std::vector<VertexData> Vertices;
        std::vector<uint32_t> Indices;
        uint32_t VertexCount;
        uint32_t IndexCount;

        void LoadFromFile(std::string fileName);
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
            return ((hash<glm::vec3>()(vertex.Pos) ^ (hash<glm::vec3>()(vertex.Color) << 1)) >> 1) ^
                   (hash<glm::vec2>()(vertex.TexCoord) << 1);
        }
    };
}
