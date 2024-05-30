#define VMA_IMPLEMENTATION
#include "vulkan_allocator.h"

#include "vulkan_initializers.h"

namespace IC {
    VulkanAllocator::VulkanAllocator(VulkanDevice &device) : _device{device} {
        VmaAllocatorCreateInfo allocatorCreateInfo = AllocatorCreateInfo(_device);
        vmaCreateAllocator(&allocatorCreateInfo, &_allocator);
    }

    VulkanAllocator::~VulkanAllocator() {
        vmaDestroyAllocator(_allocator);
    }

    void VulkanAllocator::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage,
                                       AllocatedBuffer &buffer) {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo vmaAllocInfo{};
        vmaAllocInfo.usage = memoryUsage;
        vmaAllocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

        VK_CHECK(vmaCreateBuffer(_allocator, &bufferInfo, &vmaAllocInfo, &buffer.buffer, &buffer.allocation,
                                 &buffer.allocInfo));
    }

    void VulkanAllocator::CreateBuffer(void *data, VkDeviceSize size, VkBufferUsageFlags usage,
                                       VmaMemoryUsage memoryUsage, AllocatedBuffer &buffer) {
        CreateBuffer(size, usage, memoryUsage, buffer);
        memcpy(buffer.allocInfo.pMappedData, data, size);
    }

    void VulkanAllocator::CreateImage(VkExtent3D size, VkFormat format, VkImageUsageFlags usage,
                                      AllocatedImage &image) {
        VkImageCreateInfo imageCreateInfo =
            ImageCreateInfo(size.width, size.height, format, VK_IMAGE_TILING_OPTIMAL, usage);

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        allocInfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        VK_CHECK(vmaCreateImage(_allocator, &imageCreateInfo, &allocInfo, &image.image, &image.allocation, nullptr));

        VkImageAspectFlags aspectFlag = VK_IMAGE_ASPECT_COLOR_BIT;
        if (format == VK_FORMAT_D32_SFLOAT) {
            IC_CORE_WARN("YO WE SWAGGING UP IN HERE");
            aspectFlag = VK_IMAGE_ASPECT_DEPTH_BIT;
        }

        VkImageViewCreateInfo viewInfo = ImageViewCreateInfo(format, image.image, aspectFlag);
        viewInfo.subresourceRange.levelCount = imageCreateInfo.mipLevels;

        VK_CHECK(vkCreateImageView(_device.Device(), &viewInfo, nullptr, &image.view));
    }

    void VulkanAllocator::DestroyBuffer(AllocatedBuffer &buffer) {
        vmaDestroyBuffer(_allocator, buffer.buffer, buffer.allocation);

        buffer.buffer = nullptr;
        buffer.allocation = nullptr;
    }

    void VulkanAllocator::DestroyImage(AllocatedImage &image) {
        vmaDestroyImage(_allocator, image.image, image.allocation);
        vkDestroyImageView(_device.Device(), image.view, nullptr);

        image.image = nullptr;
        image.view = nullptr;
    }
} // namespace IC