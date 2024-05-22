#pragma once

#include <ic_gameobject.h>

#include "descriptors.h"
#include "swap_chain.h"
#include "vulkan_device.h"
#include "vulkan_texture_manager.h"
#include "vulkan_types.h"

#include "vk_mem_alloc.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

namespace IC {
    // initial structures
    VmaAllocatorCreateInfo AllocatorCreateInfo(VulkanDevice &device);
    VkRenderingAttachmentInfo AttachmentInfo(VkImageView view, VkClearValue *clear,
                                             VkImageLayout layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    VkCommandBufferBeginInfo CommandBufferBeginInfo(VkCommandBufferUsageFlags flags = 0);
    VkCommandBufferSubmitInfo CommandBufferSubmitInfo(VkCommandBuffer cmd);
    VkImageCreateInfo ImageCreateInfo(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
                                      VkImageUsageFlags usage);
    VkImageViewCreateInfo ImageViewCreateInfo(VkFormat format, VkImage image, VkImageAspectFlags aspect);
    VkRenderingInfo RenderingInfo(VkExtent2D renderExtent, VkRenderingAttachmentInfo *colorAttachment,
                                  VkRenderingAttachmentInfo *depthAttachment);
    VkSubmitInfo2 SubmitInfo(VkCommandBufferSubmitInfo *cmd, VkSemaphoreSubmitInfo *signalSemaphoreInfo,
                             VkSemaphoreSubmitInfo *waitSemaphoreInfo);

    template <typename T> VkPushConstantRange PushConstants(VkShaderStageFlags flags) {
        VkPushConstantRange pushConstant{};
        pushConstant.offset = 0;
        pushConstant.size = sizeof(T);
        pushConstant.stageFlags = flags;
        return pushConstant;
    }

    // descriptors
    void WritePerObjectDescriptors(VulkanAllocator &allocator, SwapChain &swapChain, DescriptorWriter &writer,
                                   MeshRenderData &renderData);
    void WriteLightDescriptors(VulkanAllocator &allocator, size_t maxFrames, DescriptorWriter &writer,
                               std::vector<AllocatedBuffer> &lightBuffers);
    void WriteMaterialDescriptors(VulkanAllocator &allocator, size_t maxFrames, DescriptorWriter &writer,
                                  MaterialInstance &material, VulkanTextureManager &textureManager,
                                  std::vector<AllocatedBuffer> &materialBuffers);
    SceneLightDescriptors CreateSceneLightDescriptors(SceneLightData &lightData, glm::mat4 viewMat);

    // images
    void CreateImageSampler(VkDevice device, float maxAnisotropy, VkSampler &textureSampler);

    // pipelines
    std::shared_ptr<Pipeline> CreateOpaquePipeline(VkDevice device, SwapChain &swapChain,
                                                   MaterialInstance &materialData);

    // ImGui
    void InitImGui(VulkanDevice &device, GLFWwindow *window, VkDescriptorPool descriptorPool, VkFormat imageFormat);
} // namespace IC
