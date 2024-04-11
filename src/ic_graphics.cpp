#include <ic_graphics.h>
#include <ic_log.h>

#include <tiny_obj_loader.h>

namespace IC
{
    void Mesh::LoadFromFile(std::string fileName)
    {

        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, fileName.c_str()))
        {
            IC_CORE_ERROR("Failed to load {0}.", fileName);
            throw std::runtime_error(warn + err);
        }

        std::unordered_map<VertexData, uint32_t> uniqueVertices{};

        for (const auto &shape : shapes)
        {
            for (const auto &index : shape.mesh.indices)
            {
                VertexData vertex{};

                vertex.Pos = {attrib.vertices[3 * index.vertex_index + 0],
                              attrib.vertices[3 * index.vertex_index + 1],
                              attrib.vertices[3 * index.vertex_index + 2]};

                vertex.TexCoord = {attrib.texcoords[2 * index.texcoord_index + 0],
                                   1.0f - attrib.texcoords[2 * index.texcoord_index + 1]};

                vertex.Color = {1.0f, 1.0f, 1.0f};

                if (uniqueVertices.count(vertex) == 0)
                {
                    uniqueVertices[vertex] = static_cast<uint32_t>(Vertices.size());
                    Vertices.push_back(vertex);
                }

                Indices.push_back(uniqueVertices[vertex]);
            }
        }

        VertexCount = static_cast<uint32_t>(Vertices.size());
        IndexCount = static_cast<uint32_t>(Indices.size());
        assert(VertexCount >= 3 && "Vertex count must be at least 3");
    }
}
