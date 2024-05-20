#pragma once

#include "ic_material.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <imgui.h>

#include <stdexcept>
#include <string>
#include <vector>

namespace IC {
    enum class RendererType {
        None = -1,
        Vulkan
    };

    struct VertexData {
        glm::vec3 pos;
        glm::vec3 normal;
        glm::vec3 color;
        glm::vec2 texCoord;

        bool operator==(const VertexData &other) const {
            return pos == other.pos && normal == other.normal && color == other.color && texCoord == other.texCoord;
        }
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
