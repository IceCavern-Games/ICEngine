#include "descriptors.h"

#include "swap_chain.h"

namespace IC {
    void DescriptorLayoutBuilder::AddBinding(uint32_t binding, VkDescriptorType type) {
        VkDescriptorSetLayoutBinding newBinding{};
        newBinding.binding = binding;
        newBinding.descriptorCount = 1;
        newBinding.descriptorType = type;

        bindings.push_back(newBinding);
    }

    void DescriptorLayoutBuilder::Clear() {
        bindings.clear();
    }

    VkDescriptorSetLayout DescriptorLayoutBuilder::Build(VkDevice device, VkShaderStageFlags shaderStages) {
        for (VkDescriptorSetLayoutBinding &binding : bindings) {
            binding.stageFlags |= shaderStages;
        }

        VkDescriptorSetLayoutCreateInfo info = {.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
        info.pNext = nullptr;

        info.pBindings = bindings.data();
        info.bindingCount = (uint32_t)bindings.size();
        info.flags = 0;

        VkDescriptorSetLayout set;
        VK_CHECK(vkCreateDescriptorSetLayout(device, &info, nullptr, &set));

        return set;
    }

    void DescriptorWriter::WriteImage(int binding, VkImageView image, VkSampler sampler, VkImageLayout layout,
                                      VkDescriptorType type) {
        VkDescriptorImageInfo &info = imageInfos.emplace_back(
            VkDescriptorImageInfo{.sampler = sampler, .imageView = image, .imageLayout = layout});

        VkWriteDescriptorSet write = {.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};

        write.dstBinding = binding;
        write.dstSet = VK_NULL_HANDLE;
        write.descriptorCount = 1;
        write.descriptorType = type;
        write.pImageInfo = &info;

        writes.push_back(write);
    }

    void DescriptorWriter::WriteBuffer(int binding, VkBuffer buffer, size_t size, size_t offset,
                                       VkDescriptorType type) {
        VkDescriptorBufferInfo &info =
            bufferInfos.emplace_back(VkDescriptorBufferInfo{.buffer = buffer, .offset = offset, .range = size});

        VkWriteDescriptorSet write = {.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};

        write.dstBinding = binding;
        write.dstSet = VK_NULL_HANDLE;
        write.descriptorCount = 1;
        write.descriptorType = type;
        write.pBufferInfo = &info;

        writes.push_back(write);
    }

    void DescriptorWriter::Clear() {
        imageInfos.clear();
        writes.clear();
        bufferInfos.clear();
    }

    void DescriptorWriter::UpdateSet(VkDevice device, VkDescriptorSet set) {
        for (VkWriteDescriptorSet &write : writes) {
            write.dstSet = set;
        }
        vkUpdateDescriptorSets(device, (uint32_t)writes.size(), writes.data(), 0, nullptr);
    }

    void DescriptorAllocator::CreateDescriptorPool(VkDevice device, std::vector<VkDescriptorPoolSize> poolSizes,
                                                   uint32_t maxSets, VkDescriptorPoolCreateFlags flags) {
        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.flags = flags;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = maxSets;

        VK_CHECK(vkCreateDescriptorPool(device, &poolInfo, nullptr, &_pool));
    }

    void DescriptorAllocator::AllocateDescriptorSets(VkDevice device, std::vector<VkDescriptorSetLayout> layouts,
                                                     std::vector<std::vector<VkDescriptorSet>> &descriptorSets) {
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = _pool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(layouts.size());
        allocInfo.pSetLayouts = layouts.data();

        descriptorSets.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < SwapChain::MAX_FRAMES_IN_FLIGHT; i++) {
            descriptorSets[i].resize(layouts.size());
            VK_CHECK(vkAllocateDescriptorSets(device, &allocInfo, descriptorSets[i].data()));
        }
    }

    void DescriptorAllocator::DestroyDescriptorPool(VkDevice device) {
        vkDestroyDescriptorPool(device, _pool, nullptr);
    }
} // namespace IC
