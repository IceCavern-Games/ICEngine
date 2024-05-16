#pragma once

#include "vulkan_device.h"
#include "vulkan_types.h"

#include "vk_mem_alloc.h"

namespace IC {
    class VulkanAllocator {
    public:
        VulkanAllocator(VulkanDevice &device);
        ~VulkanAllocator();

        void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage,
                          AllocatedBuffer &buffer);
        void CreateBuffer(void *data, VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage,
                          AllocatedBuffer &buffer);
        void CreateImage(VkExtent3D size, VkFormat format, VkImageUsageFlags usage, AllocatedImage &image);

        void DestroyBuffer(AllocatedBuffer &buffer);
        void DestroyImage(AllocatedImage &image);

    private:
        VmaAllocator _allocator;
        VulkanDevice &_device;
    };
} // namespace IC