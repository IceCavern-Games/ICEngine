#include "swap_chain.h"

#include <ic_log.h>

#include "vulkan_initializers.h"
#include "vulkan_types.h"

#include <array>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <set>
#include <stdexcept>

namespace IC {
    SwapChain::SwapChain(VulkanDevice &deviceRef, VulkanAllocator &allocator, VkExtent2D extent,
                         std::shared_ptr<SwapChain> previous)
        : _device{deviceRef}, _allocator{allocator}, _windowExtent{extent} {
        Init(previous);
    }

    void SwapChain::Init(std::shared_ptr<SwapChain> &previous) {
        CreateSwapChain(previous);
        CreateImageViews();
        CreateRenderPass();
        CreateDepthResources();
        CreateShadowResources();
        CreateFramebuffers();
        CreateSyncObjects();
    }

    SwapChain::~SwapChain() {
        for (auto imageView : _swapChainImageViews) {
            vkDestroyImageView(_device.Device(), imageView, nullptr);
        }
        _swapChainImageViews.clear();

        if (_swapChain != nullptr) {
            vkDestroySwapchainKHR(_device.Device(), _swapChain, nullptr);
            _swapChain = nullptr;
        }

        for (int i = 0; i < _depthImages.size(); i++) {
            _allocator.DestroyImage(_depthImages[i]);
        }

        for (int i = 0; i < _shadowImages.size(); i++) {
            _allocator.DestroyImage(_shadowImages[i]);
        }

        for (auto framebuffer : _swapChainFramebuffers) {
            vkDestroyFramebuffer(_device.Device(), framebuffer, nullptr);
        }

        vkDestroyRenderPass(_device.Device(), _renderPass, nullptr);

        // cleanup synchronization objects
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroySemaphore(_device.Device(), _renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(_device.Device(), _imageAvailableSemaphores[i], nullptr);
            vkDestroyFence(_device.Device(), _inFlightFences[i], nullptr);
        }

        vkDestroyFence(_device.Device(), _immFence, nullptr);
    }

    VkResult SwapChain::AcquireNextImage(uint32_t *imageIndex) {
        vkWaitForFences(_device.Device(), 1, &_inFlightFences[_currentFrame], VK_TRUE,
                        std::numeric_limits<uint64_t>::max());

        VkResult result =
            vkAcquireNextImageKHR(_device.Device(), _swapChain, std::numeric_limits<uint64_t>::max(),
                                  _imageAvailableSemaphores[_currentFrame], // must be a not signaled semaphore
                                  VK_NULL_HANDLE, imageIndex);

        return result;
    }

    VkResult SwapChain::SubmitCommandBuffers(const VkCommandBuffer *buffers, uint32_t *imageIndex) {
        _imagesInFlight[*imageIndex] = _inFlightFences[_currentFrame];

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {_imageAvailableSemaphores[_currentFrame]};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = buffers;

        VkSemaphore signalSemaphores[] = {_renderFinishedSemaphores[_currentFrame]};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        vkResetFences(_device.Device(), 1, &_inFlightFences[_currentFrame]);
        VK_CHECK(vkQueueSubmit(_device.GraphicsQueue(), 1, &submitInfo, _inFlightFences[_currentFrame]));

        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = {_swapChain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;

        presentInfo.pImageIndices = imageIndex;

        auto result = vkQueuePresentKHR(_device.PresentQueue(), &presentInfo);

        _currentFrame = (_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

        return result;
    }

    void SwapChain::WaitForFrameFence(uint32_t *imageIndex) {
        if (_imagesInFlight[*imageIndex] != VK_NULL_HANDLE) {
            vkWaitForFences(_device.Device(), 1, &_imagesInFlight[*imageIndex], VK_TRUE, UINT64_MAX);
        }
    }

    void SwapChain::ImmediateSubmitCommandBuffers(std::function<void(VkCommandBuffer cmd)> &&function) {
        VK_CHECK(vkResetFences(_device.Device(), 1, &_immFence));
        VK_CHECK(vkResetCommandBuffer(_immCommandBuffer, 0));

        VkCommandBuffer cmd = _immCommandBuffer;
        VkCommandBufferBeginInfo beginInfo = CommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

        VK_CHECK(vkBeginCommandBuffer(_immCommandBuffer, &beginInfo));

        function(cmd);

        VK_CHECK(vkEndCommandBuffer(cmd));

        VkCommandBufferSubmitInfo cmdInfo = CommandBufferSubmitInfo(cmd);
        VkSubmitInfo2 submit = SubmitInfo(&cmdInfo, nullptr, nullptr);

        VK_CHECK(vkQueueSubmit2(_device.GraphicsQueue(), 1, &submit, _immFence));

        VK_CHECK(vkWaitForFences(_device.Device(), 1, &_immFence, VK_TRUE, 999999999));
    }

    void SwapChain::CreateSwapChain(std::shared_ptr<SwapChain> &previous) {
        SwapChainSupportDetails swapChainSupport = _device.GetSwapChainSupport();

        VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes, previous != nullptr);
        VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 &&
            imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = _device.Surface();

        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

        QueueFamilyIndices indices = _device.FindPhysicalQueueFamilies();
        uint32_t queueFamilyIndices[] = {indices.graphicsFamily, indices.presentFamily};

        if (indices.graphicsFamily != indices.presentFamily) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        } else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0;     // Optional
            createInfo.pQueueFamilyIndices = nullptr; // Optional
        }

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;

        createInfo.oldSwapchain = previous == nullptr ? VK_NULL_HANDLE : previous->_swapChain;

        VK_CHECK(vkCreateSwapchainKHR(_device.Device(), &createInfo, nullptr, &_swapChain));

        // we only specified a minimum number of images in the swap chain, so the
        // implementation is allowed to create a swap chain with more. That's why we'll
        // first query the final number of images with vkGetSwapchainImagesKHR, then resize
        // the container and finally call it again to retrieve the handles.
        vkGetSwapchainImagesKHR(_device.Device(), _swapChain, &imageCount, nullptr);
        _swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(_device.Device(), _swapChain, &imageCount, _swapChainImages.data());

        _swapChainImageFormat = surfaceFormat.format;
        _swapChainExtent = extent;
    }

    void SwapChain::CreateImageViews() {
        _swapChainImageViews.resize(_swapChainImages.size());
        for (size_t i = 0; i < _swapChainImages.size(); i++) {
            VkImageViewCreateInfo createInfo =
                ImageViewCreateInfo(_swapChainImageFormat, _swapChainImages[i], VK_IMAGE_ASPECT_COLOR_BIT);
            VK_CHECK(vkCreateImageView(_device.Device(), &createInfo, nullptr, &_swapChainImageViews[i]));
        }
    }

    void SwapChain::CreateRenderPass() {
        VkAttachmentDescription depthAttachment{};
        depthAttachment.format = FindDepthFormat();
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription colorAttachment = {};
        colorAttachment.format = GetSwapChainImageFormat();
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef = {};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;

        VkSubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.srcAccessMask = 0;
        dependency.srcStageMask =
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstSubpass = 0;
        dependency.dstStageMask =
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        VK_CHECK(vkCreateRenderPass(_device.Device(), &renderPassInfo, nullptr, &_renderPass));
    }

    void SwapChain::CreateFramebuffers() {
        _swapChainFramebuffers.resize(ImageCount());
        for (size_t i = 0; i < ImageCount(); i++) {
            std::array<VkImageView, 2> attachments = {_swapChainImageViews[i], _depthImages[i].view};

            VkExtent2D swapChainExtent = GetSwapChainExtent();
            VkFramebufferCreateInfo framebufferInfo = {};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = _renderPass;
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = swapChainExtent.width;
            framebufferInfo.height = swapChainExtent.height;
            framebufferInfo.layers = 1;

            VK_CHECK(vkCreateFramebuffer(_device.Device(), &framebufferInfo, nullptr, &_swapChainFramebuffers[i]));
        }
    }

    void SwapChain::CreateDepthResources() {
        _swapChainDepthFormat = FindDepthFormat();
        VkExtent2D swapChainExtent = GetSwapChainExtent();

        _depthImages.resize(ImageCount());

        for (int i = 0; i < _depthImages.size(); i++) {
            VkExtent3D size{};
            size.depth = 1;
            size.width = swapChainExtent.width;
            size.height = swapChainExtent.height;
            _allocator.CreateImage(size, _swapChainDepthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                                   _depthImages[i]);
        }
    }

    void SwapChain::CreateShadowResources() {
        _shadowImages.resize(ImageCount());

        for (int i = 0; i < _shadowImages.size(); i++) {
            VkExtent3D size = {};
            size.depth = 1;
            size.width = 1024;
            size.height = 1024;

            IC_CORE_WARN("CREATING SHADOW IMAGE");
            _allocator.CreateImage(size, _swapChainDepthFormat,
                                   VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                                   _shadowImages[i]);
        }
    }

    void SwapChain::CreateSyncObjects() {
        _imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        _renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        _inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
        _imagesInFlight.resize(ImageCount(), VK_NULL_HANDLE);

        VkSemaphoreCreateInfo semaphoreInfo = {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            if (vkCreateSemaphore(_device.Device(), &semaphoreInfo, nullptr, &_imageAvailableSemaphores[i]) !=
                    VK_SUCCESS ||
                vkCreateSemaphore(_device.Device(), &semaphoreInfo, nullptr, &_renderFinishedSemaphores[i]) !=
                    VK_SUCCESS ||
                vkCreateFence(_device.Device(), &fenceInfo, nullptr, &_inFlightFences[i]) != VK_SUCCESS) {
                IC_CORE_ERROR("Failed to create synchronization objects for a frame.");
                throw std::runtime_error("Failed to create synchronization objects for a frame.");
            }
        }

        // for immediate submit
        VK_CHECK(vkCreateFence(_device.Device(), &fenceInfo, nullptr, &_immFence));
    }

    VkSurfaceFormatKHR SwapChain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats) {
        for (const auto &availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
                availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    VkPresentModeKHR SwapChain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes,
                                                      bool previousSwapChain) {
        for (const auto &availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                if (!previousSwapChain) {
                    IC_CORE_INFO("Present mode: Mailbox");
                }
                return availablePresentMode;
            }
        }

        // for (const auto &availablePresentMode : availablePresentModes) {
        //   if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
        //     IC_CORE_INFO("Present mode: Immediate");
        //     return availablePresentMode;
        //   }
        // }
        if (!previousSwapChain) {
            IC_CORE_INFO("Present mode: V-Sync");
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D SwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        } else {
            VkExtent2D actualExtent = _windowExtent;
            actualExtent.width = std::max(capabilities.minImageExtent.width,
                                          std::min(capabilities.maxImageExtent.width, actualExtent.width));
            actualExtent.height = std::max(capabilities.minImageExtent.height,
                                           std::min(capabilities.maxImageExtent.height, actualExtent.height));

            return actualExtent;
        }
    }

    VkFormat SwapChain::FindDepthFormat() {
        return _device.FindSupportedFormat(
            {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT}, VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    }

} // namespace IC
