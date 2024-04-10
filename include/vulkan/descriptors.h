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

        void add_binding(uint32_t binding, VkDescriptorType type);
        void clear();
        VkDescriptorSetLayout build(VkDevice device, VkShaderStageFlags shaderStages);
    };

    struct DescriptorWriter
    {
        std::deque<VkDescriptorImageInfo> imageInfos;
        std::deque<VkDescriptorBufferInfo> bufferInfos;
        std::vector<VkWriteDescriptorSet> writes;

        void write_image(int binding, VkImageView image, VkSampler sampler, VkImageLayout layout, VkDescriptorType type);
        void write_buffer(int binding, VkBuffer buffer, size_t size, size_t offset, VkDescriptorType type);

        void clear();
        void update_set(VkDevice device, VkDescriptorSet set);
    };

    // Descriptor Pool Allocator
    struct DescriptorAllocator
    {
    public:
        void createDescriptorPool(VkDevice device, std::vector<VkDescriptorPoolSize> poolSizes, uint32_t maxSets);
        void allocateDescriptorSets(VkDevice device, VkDescriptorSetLayout layout, std::vector<VkDescriptorSet> &descriptorSets);
        void destroyDescriptorPool(VkDevice device);

    private:
        VkDescriptorPool pool;
    };
}