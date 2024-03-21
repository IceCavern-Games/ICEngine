#pragma once

#include "../ic_device.hpp"
#include "../ic_types.hpp"

namespace util {
/*void copyImageToImage(VkCommandBuffer commandBuffer, VkImage source, VkImage destination,
                      VkExtent2D srcSize, VkExtent2D dstSize);
void loadTextureImage(render::ICDevice *lveDevice, std::string texturePath,
                      render::AllocatedImage &outImage);*/
void transitionImageLayout(VkCommandBuffer commandBuffer, VkImage image, VkFormat format,
                           VkImageLayout oldLayout, VkImageLayout newLayout);
} // namespace util