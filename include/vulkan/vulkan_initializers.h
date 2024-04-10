#pragma once

#include <swap_chain.h>
#include <vulkan_device.h>
#include <vulkan_types.h>

namespace IC::Renderer::Init
{
    // initial structures
    VkRenderingAttachmentInfo attachmentInfo(VkImageView view, VkClearValue *clear, VkImageLayout layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    VkCommandBufferBeginInfo commandBufferBeginInfo(VkCommandBufferUsageFlags flags = 0);
    VkCommandBufferSubmitInfo commandBufferSubmitInfo(VkCommandBuffer cmd);
    VkRenderingInfo renderingInfo(VkExtent2D renderExtent, VkRenderingAttachmentInfo *colorAttachment, VkRenderingAttachmentInfo *depthAttachment);
    VkSubmitInfo2 submitInfo(VkCommandBufferSubmitInfo *cmd, VkSemaphoreSubmitInfo *signalSemaphoreInfo, VkSemaphoreSubmitInfo *waitSemaphoreInfo);

    template <typename T>
    VkPushConstantRange pushConstants(VkShaderStageFlags flags)
    {
        VkPushConstantRange pushConstant{};
        pushConstant.offset = 0;
        pushConstant.size = sizeof(T);
        pushConstant.stageFlags = flags;
        return pushConstant;
    }

    // images
    void createImage(VulkanDevice *device, uint32_t width, uint32_t height, VkFormat format,
                     VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
                     AllocatedImage &image);
    void createImageSampler(VkDevice device, float maxAnisotropy, VkSampler &textureSampler);

    // pipelines
    std::shared_ptr<Pipeline> createOpaquePipeline(VkDevice device, SwapChain &swapChain, ICMaterial &materialData);
}