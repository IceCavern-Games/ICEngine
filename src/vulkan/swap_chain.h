#pragma once

#include "vulkan_device.h"

#include <vulkan/vulkan.h>

#include <functional>
#include <string>
#include <vector>

namespace IC {
    class SwapChain {
    public:
        static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

        SwapChain(VulkanDevice &deviceRef, VkExtent2D windowExtent);
        ~SwapChain();

        SwapChain(const SwapChain &) = delete;
        void operator=(const SwapChain &) = delete;

        size_t GetCurrentFrame() { return _currentFrame; }
        VkFramebuffer GetFrameBuffer(int index) { return _swapChainFramebuffers[index]; }
        VkRenderPass GetRenderPass() { return _renderPass; }
        VkImageView GetImageView(int index) { return _swapChainImageViews[index]; }
        VkImage GetImage(int index) { return _swapChainImages[index]; }
        VkImageView GetDepthImageView(int index) { return _depthImageViews[index]; }
        VkImage GetDepthImage(int index) { return _depthImages[index]; }
        size_t ImageCount() { return _swapChainImages.size(); }
        VkFormat GetSwapChainDepthFormat() { return _swapChainDepthFormat; }
        VkFormat GetSwapChainImageFormat() { return _swapChainImageFormat; }
        VkExtent2D GetSwapChainExtent() { return _swapChainExtent; }
        uint32_t Width() { return _swapChainExtent.width; }
        uint32_t Height() { return _swapChainExtent.height; }

        float ExtentAspectRatio() {
            return static_cast<float>(_swapChainExtent.width) /
                   static_cast<float>(_swapChainExtent.height);
        }
        VkFormat FindDepthFormat();

        VkResult AcquireNextImage(uint32_t *imageIndex);
        VkResult SubmitCommandBuffers(const VkCommandBuffer *buffers, uint32_t *imageIndex);
        void WaitForFrameFence(uint32_t *imageIndex);
        void ImmediateSubmitCommandBuffers(const VkCommandBuffer buffer,
                                           std::function<void(VkCommandBuffer cmd)> &&function);

    private:
        void CreateSwapChain();
        void CreateImageViews();
        void CreateDepthResources();
        void CreateRenderPass();
        void CreateFramebuffers();
        void CreateSyncObjects();

        // Helper functions
        VkSurfaceFormatKHR
        ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
        VkPresentModeKHR
        ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
        VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

        VkFormat _swapChainImageFormat;
        VkFormat _swapChainDepthFormat;
        VkExtent2D _swapChainExtent;

        std::vector<VkFramebuffer> _swapChainFramebuffers;
        VkRenderPass _renderPass;

        std::vector<VkImage> _depthImages;
        std::vector<VkDeviceMemory> _depthImageMemorys;
        std::vector<VkImageView> _depthImageViews;
        std::vector<VkImage> _swapChainImages;
        std::vector<VkImageView> _swapChainImageViews;

        VulkanDevice &_device;
        VkExtent2D _windowExtent;

        VkSwapchainKHR _swapChain;

        std::vector<VkSemaphore> _imageAvailableSemaphores;
        std::vector<VkSemaphore> _renderFinishedSemaphores;
        std::vector<VkFence> _inFlightFences;
        std::vector<VkFence> _imagesInFlight;
        size_t _currentFrame = 0;

        VkFence _immFence;
    };

} // namespace IC
