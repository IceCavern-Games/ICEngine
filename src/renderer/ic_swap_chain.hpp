#pragma once

#include "ic_device.hpp"

// vulkan headers
#include <vulkan/vulkan.h>

// std lib headers
#include <functional>
#include <string>
#include <vector>

namespace render {

class ICSwapChain {
public:
    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

    ICSwapChain(ICDevice &deviceRef, VkExtent2D windowExtent);
    ~ICSwapChain();

    ICSwapChain(const ICSwapChain &) = delete;
    void operator=(const ICSwapChain &) = delete;

    size_t getCurrentFrame() { return currentFrame; }
    VkFramebuffer getFrameBuffer(int index) { return swapChainFramebuffers[index]; }
    VkRenderPass getRenderPass() { return renderPass; }
    VkImageView getImageView(int index) { return swapChainImageViews[index]; }
    VkImage getImage(int index) { return swapChainImages[index]; }
    VkImageView getDepthImageView(int index) { return depthImageViews[index]; }
    VkImage getDepthImage(int index) { return depthImages[index]; }
    size_t imageCount() { return swapChainImages.size(); }
    VkFormat getSwapChainDepthFormat() { return swapChainDepthFormat; }
    VkFormat getSwapChainImageFormat() { return swapChainImageFormat; }
    VkExtent2D getSwapChainExtent() { return swapChainExtent; }
    uint32_t width() { return swapChainExtent.width; }
    uint32_t height() { return swapChainExtent.height; }

    float extentAspectRatio() {
        return static_cast<float>(swapChainExtent.width) /
               static_cast<float>(swapChainExtent.height);
    }
    VkFormat findDepthFormat();

    VkResult acquireNextImage(uint32_t *imageIndex);
    VkResult submitCommandBuffers(const VkCommandBuffer *buffers, uint32_t *imageIndex);
    void waitForFrameFence(uint32_t *imageIndex);
    void
    immediateSubmitCommandBuffers(const VkCommandBuffer buffer,
                                  std::function<void(VkCommandBuffer cmd)> &&function);

private:
    void createSwapChain();
    void createImageViews();
    void createDepthResources();
    void createRenderPass();
    void createFramebuffers();
    void createSyncObjects();

    // Helper functions
    VkSurfaceFormatKHR
    chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
    VkPresentModeKHR
    chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

    VkFormat swapChainImageFormat;
    VkFormat swapChainDepthFormat;
    VkExtent2D swapChainExtent;

    std::vector<VkFramebuffer> swapChainFramebuffers;
    VkRenderPass renderPass;

    std::vector<VkImage> depthImages;
    std::vector<VkDeviceMemory> depthImageMemorys;
    std::vector<VkImageView> depthImageViews;
    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;

    ICDevice &device;
    VkExtent2D windowExtent;

    VkSwapchainKHR swapChain;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    std::vector<VkFence> imagesInFlight;
    size_t currentFrame = 0;

    VkFence immFence;
};

} // namespace render
