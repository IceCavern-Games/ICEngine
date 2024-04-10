#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include <ic_graphics.h>
#include <vulkan/vk_enum_string_helper.h>

#include <array>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <vector>

#define VK_CHECK(x)                                         \
    do                                                      \
    {                                                       \
        VkResult err = x;                                   \
        if (err)                                            \
        {                                                   \
            throw std::runtime_error(string_VkResult(err)); \
            abort();                                        \
        }                                                   \
    } while (0)

namespace IC::Renderer
{
    struct AllocatedBuffer
    {
        VkBuffer buffer;
        VkDeviceMemory memory;
        void *mapped_memory;
    };

    struct AllocatedImage
    {
        VkImage image;
        VkImageView view;
        VkDeviceMemory memory;
    };

    struct MVPObject
    {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
    };

    struct Pipeline
    {
        VkPipeline pipeline;
        VkPipelineLayout layout;
        std::vector<VkShaderModule> shaderModules;
        VkDescriptorSetLayout descriptorSetLayout;
        bool transparent = false;

        bool operator<(const Pipeline &other) const
        {
            return pipeline < other.pipeline && transparent <= other.transparent;
        }
    };

    struct MeshRenderData
    {
        ICMesh &meshData;
        ICMaterial &materialData;
        std::shared_ptr<Pipeline> renderPipeline;
        std::vector<VkDescriptorSet> descriptorSets;
        AllocatedBuffer vertexBuffer;
        AllocatedBuffer indexBuffer;
        std::vector<AllocatedBuffer> mvp_buffers;
        std::vector<AllocatedBuffer> constants_buffers;

        void bind(VkCommandBuffer cBuffer, VkPipelineLayout pipelineLayout, size_t currentFrame)
        {
            VkBuffer vertexBuffers[] = {vertexBuffer.buffer};
            VkDeviceSize offsets[] = {0};
            vkCmdBindVertexBuffers(cBuffer, 0, 1, vertexBuffers, offsets);
            vkCmdBindIndexBuffer(cBuffer, indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);
            vkCmdBindDescriptorSets(cBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[currentFrame], 0, nullptr);
        }

        void draw(VkCommandBuffer cBuffer)
        {
            vkCmdDrawIndexed(cBuffer, meshData.index_count, 1, 0, 0, 0);
        }

        void update_mvp_buffer(MVPObject uniformBuffer, uint32_t currentImage)
        {
            memcpy(mvp_buffers[currentImage].mapped_memory, &uniformBuffer, sizeof(uniformBuffer));
        }
    };

    static VkVertexInputBindingDescription getVertexBindingDescription()
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(VertexData);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 3> getVertexAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(VertexData, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(VertexData, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(VertexData, texCoord);
        return attributeDescriptions;
    }
}