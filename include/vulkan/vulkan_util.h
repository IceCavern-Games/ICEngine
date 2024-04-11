#pragma once

#include <vulkan_types.h>
#include <vulkan_device.h>

namespace IC::Renderer::Util
{
    void create_and_fill_buffer(VulkanDevice &device, const void *srcData, VkDeviceSize bufferSize, VkBufferUsageFlags buffer_usage_flags, VkMemoryPropertyFlags memory_property_flags, AllocatedBuffer &allocatedBuffer);
    VkDescriptorType material_input_type_mapping(MaterialInputType inputType);

    void copy_image_to_image(VkCommandBuffer commandBuffer, VkImage source, VkImage destination, VkExtent2D srcSize, VkExtent2D dstSize);
    void transition_image_layout(VkCommandBuffer commandBuffer, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    // void load_texture_image(VulkanDevice &device, std::string texturePath, AllocatedImage &outImage);
}