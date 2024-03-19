#pragma once

#include <vulkan/vulkan.h>

namespace init {
VkRenderingAttachmentInfo
attachmentInfo(VkImageView view, VkClearValue *clear,
               VkImageLayout layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
VkCommandBufferBeginInfo commandBufferBeginInfo(VkCommandBufferUsageFlags flags = 0);
VkCommandBufferSubmitInfo commandBufferSubmitInfo(VkCommandBuffer cmd);
VkRenderingInfo renderingInfo(VkExtent2D renderExtent,
                              VkRenderingAttachmentInfo *colorAttachment,
                              VkRenderingAttachmentInfo *depthAttachment);
VkSubmitInfo2 submitInfo(VkCommandBufferSubmitInfo *cmd,
                         VkSemaphoreSubmitInfo *signalSemaphoreInfo,
                         VkSemaphoreSubmitInfo *waitSemaphoreInfo);

template <typename T> VkPushConstantRange pushConstants(VkShaderStageFlags flags) {
    VkPushConstantRange pushConstant{};
    pushConstant.offset = 0;
    pushConstant.size = sizeof(T);
    pushConstant.stageFlags = flags;
    return pushConstant;
}
} // namespace init