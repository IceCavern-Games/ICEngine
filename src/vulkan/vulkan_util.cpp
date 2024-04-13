#include "vulkan_util.h"

#include "vulkan_initializers.h"

#include <cstring>

namespace IC {
    void CreateAndFillBuffer(VulkanDevice &device, const void *srcData, VkDeviceSize bufferSize,
                             VkBufferUsageFlags bufferUsageFlags, VkMemoryPropertyFlags memoryPropertyFlags,
                             AllocatedBuffer &allocatedBuffer) {
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;

        device.CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer,
                            stagingBufferMemory);
        void *data;
        vkMapMemory(device.Device(), stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, srcData, (size_t)bufferSize);
        vkUnmapMemory(device.Device(), stagingBufferMemory);

        device.CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | bufferUsageFlags, memoryPropertyFlags,
                            allocatedBuffer.buffer, allocatedBuffer.memory);

        device.CopyBuffer(stagingBuffer, allocatedBuffer.buffer, bufferSize);

        vkDestroyBuffer(device.Device(), stagingBuffer, nullptr);
        vkFreeMemory(device.Device(), stagingBufferMemory, nullptr);
    }

    VkDescriptorType MaterialInputTypeMapping(MaterialInputType inputType) {
        switch (inputType) {
        case MaterialInputType::Color:
            return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        case MaterialInputType::Texture:
            return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        default:
            return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        }
    }

    void CopyImageToImage(VkCommandBuffer commandBuffer, VkImage source, VkImage destination, VkExtent2D srcSize,
                          VkExtent2D dstSize) {
        VkImageBlit2 blitRegion{.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2, .pNext = nullptr};

        blitRegion.srcOffsets[1].x = srcSize.width;
        blitRegion.srcOffsets[1].y = srcSize.height;
        blitRegion.srcOffsets[1].z = 1;

        blitRegion.dstOffsets[1].x = dstSize.width;
        blitRegion.dstOffsets[1].y = dstSize.height;
        blitRegion.dstOffsets[1].z = 1;

        blitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blitRegion.srcSubresource.baseArrayLayer = 0;
        blitRegion.srcSubresource.layerCount = 1;
        blitRegion.srcSubresource.mipLevel = 0;

        blitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blitRegion.dstSubresource.baseArrayLayer = 0;
        blitRegion.dstSubresource.layerCount = 1;
        blitRegion.dstSubresource.mipLevel = 0;

        VkBlitImageInfo2 blitInfo{.sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2, .pNext = nullptr};
        blitInfo.dstImage = destination;
        blitInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        blitInfo.srcImage = source;
        blitInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        blitInfo.filter = VK_FILTER_LINEAR;
        blitInfo.regionCount = 1;
        blitInfo.pRegions = &blitRegion;

        vkCmdBlitImage2(commandBuffer, &blitInfo);
    }

    void TransitionImageLayout(VkCommandBuffer commandBuffer, VkImage image, VkFormat format, VkImageLayout oldLayout,
                               VkImageLayout newLayout) {
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = 0;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
                   newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } else {
            barrier.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_MEMORY_WRITE_BIT | VK_ACCESS_MEMORY_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
            destinationStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
        }

        vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    }
    // todo
    /*
    void LoadTextureImage(VulkanDevice &device, std::string texturePath, AllocatedImage &outImage)
    {
        int texWidth, texHeight, texChannels;
        stbi_uc *pixels = stbi_load(texturePath.c_str(), &texWidth, &texHeight, &texChannels,
    STBI_rgb_alpha); VkDeviceSize imageSize = texWidth * texHeight * 4;

        if (!pixels)
        {
            IC_CORE_ERROR("Failed to load texture image.");
            throw std::runtime_error("Failed to load texture image.");
        }

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;

        device.createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                            stagingBuffer, stagingBufferMemory);

        void *data;
        vkMapMemory(device.Device(), stagingBufferMemory, 0, imageSize, 0, &data);
        memcpy(data, pixels, static_cast<size_t>(imageSize));
        vkUnmapMemory(device.Device(), stagingBufferMemory);
        stbi_image_free(pixels);

        VkCommandBuffer commandBuffer = device.BeginSingleTimeCommands();
        Init::createImage(&device, texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB,
                          VK_IMAGE_TILING_OPTIMAL,
                          VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, outImage);
        TransitionImageLayout(commandBuffer, outImage.image, VK_FORMAT_R8G8B8A8_SRGB,
                                VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        device.EndSingleTimeCommands(commandBuffer);

        device.CopyBufferToImage(stagingBuffer, outImage.image, static_cast<uint32_t>(texWidth),
                                 static_cast<uint32_t>(texHeight), 1);
        commandBuffer = device.BeginSingleTimeCommands();
        TransitionImageLayout(commandBuffer, outImage.image, VK_FORMAT_R8G8B8A8_SRGB,
                                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        device.EndSingleTimeCommands(commandBuffer);

        vkDestroyBuffer(device.Device(), stagingBuffer, nullptr);
        vkFreeMemory(device.Device(), stagingBufferMemory, nullptr);
    }
    */
} // namespace IC
