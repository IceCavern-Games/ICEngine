#include "initializers.hpp"

namespace init {
VkRenderingAttachmentInfo attachmentInfo(VkImageView view, VkClearValue *clear,
                                         VkImageLayout layout) {
    VkRenderingAttachmentInfo info = {.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO};
    info.pNext = nullptr;

    info.imageView = view;
    info.imageLayout = layout;
    info.loadOp = clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
    info.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    if (clear) {
        info.clearValue = *clear;
    }

    return info;
}

VkCommandBufferAllocateInfo commandBufferAllocateInfo(VkCommandPool pool, uint32_t bufferCount) {
    VkCommandBufferAllocateInfo info = {.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    info.pNext = nullptr;
    info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    info.commandPool = pool;
    info.commandBufferCount = bufferCount;

    return info;
}

VkCommandBufferBeginInfo commandBufferBeginInfo(VkCommandBufferUsageFlags flags /*= 0*/) {
    VkCommandBufferBeginInfo info = {.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    info.pNext = nullptr;
    info.pInheritanceInfo = nullptr;
    info.flags = flags;

    return info;
}

VkCommandBufferSubmitInfo commandBufferSubmitInfo(VkCommandBuffer cmd) {
    VkCommandBufferSubmitInfo info = {.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO};
    info.pNext = nullptr;
    info.commandBuffer = cmd;
    info.deviceMask = 0;

    return info;
}

VkRenderingInfo renderingInfo(VkExtent2D renderExtent, VkRenderingAttachmentInfo *colorAttachment,
                              VkRenderingAttachmentInfo *depthAttachment) {
    VkRenderingInfo info{.sType = VK_STRUCTURE_TYPE_RENDERING_INFO};
    info.pNext = nullptr;
    info.renderArea = VkRect2D{VkOffset2D{0, 0}, renderExtent};
    info.layerCount = 1;
    info.colorAttachmentCount = 1;
    info.pColorAttachments = colorAttachment;
    info.pDepthAttachment = depthAttachment;
    info.pStencilAttachment = nullptr;

    return info;
}

VkSubmitInfo2 submitInfo(VkCommandBufferSubmitInfo *cmd, VkSemaphoreSubmitInfo *signalSemaphoreInfo,
                         VkSemaphoreSubmitInfo *waitSemaphoreInfo) {
    VkSubmitInfo2 info = {.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2};
    info.pNext = nullptr;
    info.waitSemaphoreInfoCount = waitSemaphoreInfo == nullptr ? 0 : 1;
    info.pWaitSemaphoreInfos = waitSemaphoreInfo;
    info.signalSemaphoreInfoCount = signalSemaphoreInfo == nullptr ? 0 : 1;
    info.pSignalSemaphoreInfos = signalSemaphoreInfo;
    info.commandBufferInfoCount = 1;
    info.pCommandBufferInfos = cmd;

    return info;
}
} // namespace init