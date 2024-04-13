#pragma once

#include "vulkan_device.h"
#include "vulkan_types.h"

namespace IC {
    void CreateAndFillBuffer(VulkanDevice &device, const void *srcData, VkDeviceSize bufferSize,
                             VkBufferUsageFlags bufferUsageFlags, VkMemoryPropertyFlags memoryPropertyFlags,
                             AllocatedBuffer &allocatedBuffer);
    VkDescriptorType MaterialInputTypeMapping(MaterialInputType inputType);

    void CopyImageToImage(VkCommandBuffer commandBuffer, VkImage source, VkImage destination, VkExtent2D srcSize,
                          VkExtent2D dstSize);
    void TransitionImageLayout(VkCommandBuffer commandBuffer, VkImage image, VkFormat format, VkImageLayout oldLayout,
                               VkImageLayout newLayout);
    // void LoadTextureImage(VulkanDevice &device, std::string texturePath, AllocatedImage
    // &outImage);
} // namespace IC