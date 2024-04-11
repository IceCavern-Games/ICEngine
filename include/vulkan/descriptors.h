#pragma once

#include <vulkan_types.h>

#include <vector>
#include <deque>

namespace IC::Renderer
{
    // Builds Descriptor Set Layouts
    struct DescriptorLayoutBuilder
    {
        std::vector<VkDescriptorSetLayoutBinding> bindings;

        void AddBinding(uint32_t binding, VkDescriptorType type);
        void Clear();
        VkDescriptorSetLayout Build(VkDevice device, VkShaderStageFlags shaderStages);
    };

    struct DescriptorWriter
    {
        std::deque<VkDescriptorImageInfo> imageInfos;
        std::deque<VkDescriptorBufferInfo> bufferInfos;
        std::vector<VkWriteDescriptorSet> writes;

        void WriteImage(int binding, VkImageView image, VkSampler sampler, VkImageLayout layout, VkDescriptorType type);
        void WriteBuffer(int binding, VkBuffer buffer, size_t size, size_t offset, VkDescriptorType type);

        void Clear();
        void UpdateSet(VkDevice device, VkDescriptorSet set);
    };

    // Descriptor Pool Allocator
    struct DescriptorAllocator
    {
    public:
        void CreateDescriptorPool(VkDevice device, std::vector<VkDescriptorPoolSize> poolSizes, uint32_t maxSets);
        void AllocateDescriptorSets(VkDevice device, VkDescriptorSetLayout layout, std::vector<VkDescriptorSet> &descriptorSets);
        void DestroyDescriptorPool(VkDevice device);

    private:
        VkDescriptorPool _pool;
    };
}