#pragma once

#include "swap_chain.h"
#include "vulkan_device.h"
#include "vulkan_types.h"

namespace IC::Init
{
    // initial structures
    VkRenderingAttachmentInfo AttachmentInfo(VkImageView view, VkClearValue *clear, VkImageLayout layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    VkCommandBufferBeginInfo CommandBufferBeginInfo(VkCommandBufferUsageFlags flags = 0);
    VkCommandBufferSubmitInfo CommandBufferSubmitInfo(VkCommandBuffer cmd);
    VkRenderingInfo RenderingInfo(VkExtent2D renderExtent, VkRenderingAttachmentInfo *colorAttachment, VkRenderingAttachmentInfo *depthAttachment);
    VkSubmitInfo2 SubmitInfo(VkCommandBufferSubmitInfo *cmd, VkSemaphoreSubmitInfo *signalSemaphoreInfo, VkSemaphoreSubmitInfo *waitSemaphoreInfo);

    template <typename T>
    VkPushConstantRange PushConstants(VkShaderStageFlags flags)
    {
        VkPushConstantRange pushConstant{};
        pushConstant.offset = 0;
        pushConstant.size = sizeof(T);
        pushConstant.stageFlags = flags;
        return pushConstant;
    }

    // images
    void CreateImage(VulkanDevice *device, uint32_t width, uint32_t height, VkFormat format,
                     VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
                     AllocatedImage &image);
    void CreateImageSampler(VkDevice device, float maxAnisotropy, VkSampler &textureSampler);

    // pipelines
    std::shared_ptr<Pipeline> CreateOpaquePipeline(VkDevice device, SwapChain &swapChain, Material &materialData);
}
