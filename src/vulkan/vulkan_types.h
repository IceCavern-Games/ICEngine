#pragma once

#include <ic_graphics.h>
#include <ic_log.h>

#include "vulkan_constants.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <vulkan/vk_enum_string_helper.h>

#include <array>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <vector>

#define VK_CHECK(x)                                                                                                    \
    do {                                                                                                               \
        VkResult err = x;                                                                                              \
        if (err) {                                                                                                     \
            IC_CORE_ERROR("{0}", string_VkResult(err));                                                                \
            throw std::runtime_error(string_VkResult(err));                                                            \
            abort();                                                                                                   \
        }                                                                                                              \
    } while (0)

namespace IC {
    struct AllocatedBuffer {
        VkBuffer buffer;
        VkDeviceMemory memory;
        void *mappedMemory;
    };

    struct AllocatedImage {
        VkImage image;
        VkImageView view;
        VkDeviceMemory memory;
    };

    struct CameraDescriptors {
        glm::mat4 proj;
    };

    // size = 128 bytes
    struct TransformationPushConstants {
        glm::mat4 model;
        glm::mat4 view;
    };

    struct DirectionalLightDescriptors {
        alignas(16) glm::vec3 dir;
        alignas(16) glm::vec3 amb;
        alignas(16) glm::vec3 diff;
        alignas(16) glm::vec3 spec;
    };

    struct PointLightDescriptors {
        alignas(16) glm::vec3 pos;
        alignas(16) glm::vec3 amb;
        alignas(16) glm::vec3 diff;
        alignas(16) glm::vec3 spec;
        glm::float32 cons;
        glm::float32 lin;
        glm::float32 quad;
    };

    struct SceneLightDescriptors {
        DirectionalLightDescriptors directionalLight;
        std::array<PointLightDescriptors, MAX_POINT_LIGHTS> pointLights;
        glm::uint32 numPointLights;
    };

    struct Pipeline {
        VkPipeline pipeline;
        VkPipelineLayout layout;
        std::vector<VkShaderModule> shaderModules;
        std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
        MaterialFlags materialFlags;

        bool operator<(const Pipeline &other) const {
            return pipeline < other.pipeline &&
                   (materialFlags & MaterialFlags::Transparent) <= (other.materialFlags & MaterialFlags::Transparent);
        }
    };

    struct MeshRenderData {
        Mesh &meshData;
        MaterialInstance &materialData;
        std::shared_ptr<Pipeline> renderPipeline;
        std::vector<std::vector<VkDescriptorSet>> descriptorSets;
        AllocatedBuffer vertexBuffer;
        AllocatedBuffer indexBuffer;
        std::vector<AllocatedBuffer> mvpBuffers;
        std::vector<AllocatedBuffer> lightsBuffers;
        std::vector<AllocatedBuffer> materialBuffers;

        void Bind(VkCommandBuffer cBuffer, VkPipelineLayout pipelineLayout, size_t currentFrame) {
            VkBuffer vertexBuffers[] = {vertexBuffer.buffer};
            VkDeviceSize offsets[] = {0};
            vkCmdBindVertexBuffers(cBuffer, 0, 1, vertexBuffers, offsets);
            vkCmdBindIndexBuffer(cBuffer, indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);
            vkCmdBindDescriptorSets(cBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0,
                                    descriptorSets[currentFrame].size(), descriptorSets[currentFrame].data(), 0,
                                    nullptr);
        }

        void Draw(VkCommandBuffer cBuffer) { vkCmdDrawIndexed(cBuffer, meshData.indexCount, 1, 0, 0, 0); }

        void UpdateMvpBuffer(CameraDescriptors uniformBuffer, uint32_t currentImage) {
            memcpy(mvpBuffers[currentImage].mappedMemory, &uniformBuffer, sizeof(uniformBuffer));
        }

        void UpdateMaterialBuffer(uint32_t currentImage) {
            VkDeviceSize offset = 0;
            for (auto &[index, binding] : materialData.BindingValues()) {
                if (binding->binding.bindingType == BindingType::Uniform) {
                    memcpy(static_cast<char *>(materialBuffers[currentImage].mappedMemory) + offset, binding->value,
                           binding->size);
                    offset += binding->size;
                }
            }
        }

        template <typename T> void UpdateUniformBuffer(T &data, AllocatedBuffer &buffer) {
            memcpy(buffer.mappedMemory, &data, sizeof(T));
        }
    };

    static VkVertexInputBindingDescription GetVertexBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(VertexData);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 4> GetVertexAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions{};
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(VertexData, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(VertexData, normal);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(VertexData, color);

        attributeDescriptions[3].binding = 0;
        attributeDescriptions[3].location = 3;
        attributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[3].offset = offsetof(VertexData, texCoord);
        return attributeDescriptions;
    }
} // namespace IC
