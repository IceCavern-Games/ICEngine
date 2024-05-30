#pragma once

#include "vulkan_device.h"
#include "vulkan_types.h"

namespace IC {
    void CopyImageToImage(VkCommandBuffer commandBuffer, VkImage source, VkImage destination, VkExtent2D srcSize,
                          VkExtent2D dstSize);
    void TransitionImageLayout(VkCommandBuffer commandBuffer, VkImage image, VkFormat format, VkImageLayout oldLayout,
                               VkImageLayout newLayout, VkImageAspectFlags flags = VK_IMAGE_ASPECT_COLOR_BIT);
    // destructors
    void DestroyPipeline(VkDevice device, const Pipeline &pipeline);
} // namespace IC
