#include <ic_graphics.h>

#include <tiny_obj_loader.h>

namespace IC
{
    void ICMesh::load_from_file(std::string file_name)
    {

        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, file_name.c_str()))
        {
            throw std::runtime_error(warn + err);
        }

        std::unordered_map<VertexData, uint32_t> unique_vertices{};

        for (const auto &shape : shapes)
        {
            for (const auto &index : shape.mesh.indices)
            {
                VertexData vertex{};

                vertex.pos = {attrib.vertices[3 * index.vertex_index + 0],
                              attrib.vertices[3 * index.vertex_index + 1],
                              attrib.vertices[3 * index.vertex_index + 2]};

                vertex.texCoord = {attrib.texcoords[2 * index.texcoord_index + 0],
                                   1.0f - attrib.texcoords[2 * index.texcoord_index + 1]};

                vertex.color = {1.0f, 1.0f, 1.0f};

                if (unique_vertices.count(vertex) == 0)
                {
                    unique_vertices[vertex] = static_cast<uint32_t>(vertices.size());
                    vertices.push_back(vertex);
                }

                indices.push_back(unique_vertices[vertex]);
            }
        }

        vertex_count = static_cast<uint32_t>(vertices.size());
        index_count = static_cast<uint32_t>(indices.size());
        assert(vertex_count >= 3 && "Vertex count must be at least 3");
    }
}