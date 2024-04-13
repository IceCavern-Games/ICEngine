#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_beta.h>

#include <string>
#include <vector>

namespace IC {

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    struct QueueFamilyIndices {
        uint32_t graphicsFamily;
        uint32_t presentFamily;
        bool graphicsFamilyHasValue = false;
        bool presentFamilyHasValue = false;
        bool IsComplete() { return graphicsFamilyHasValue && presentFamilyHasValue; }
    };

    class VulkanDevice {
    public:
#ifdef NDEBUG
        const bool enableValidationLayers = false;
#else
        const bool enableValidationLayers = true;
#endif
        VulkanDevice(GLFWwindow *window);
        ~VulkanDevice();

        // Not copyable or movable
        VulkanDevice(const VulkanDevice &) = delete;
        VulkanDevice operator=(const VulkanDevice &) = delete;
        VulkanDevice(VulkanDevice &&) = delete;
        VulkanDevice &operator=(VulkanDevice &&) = delete;

        VkCommandPool GetCommandPool() {
            return _commandPool;
        }
        VkDevice Device() {
            return _device;
        }
        VkPhysicalDevice PhysicalDevice() {
            return _physicalDevice;
        }
        VkInstance Instance() {
            return _instance;
        }
        VkSurfaceKHR Surface() {
            return _surface;
        }
        VkQueue GraphicsQueue() {
            return _graphicsQueue;
        }
        VkQueue PresentQueue() {
            return _presentQueue;
        }

        SwapChainSupportDetails GetSwapChainSupport() {
            return QuerySwapChainSupport(_physicalDevice);
        }
        uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        QueueFamilyIndices FindPhysicalQueueFamilies() {
            return FindQueueFamilies(_physicalDevice);
        }
        VkFormat FindSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling,
                                     VkFormatFeatureFlags features);

        // Buffer Helper Functions
        void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                          VkBuffer &buffer, VkDeviceMemory &bufferMemory);
        VkCommandBuffer BeginSingleTimeCommands();
        void EndSingleTimeCommands(VkCommandBuffer commandBuffer);
        void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
        void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);
        VkImageView CreateImageView(VkImage image, VkFormat format);

        void CreateImageWithInfo(const VkImageCreateInfo &imageInfo, VkMemoryPropertyFlags properties, VkImage &image,
                                 VkDeviceMemory &imageMemory);

        VkPhysicalDeviceProperties properties;

    private:
        void CreateInstance();
        void SetupDebugMessenger();
        void CreateSurface();
        void PickPhysicalDevice();
        void CreateLogicalDevice();
        void CreateCommandPool();

        // helper functions
        bool IsDeviceSuitable(VkPhysicalDevice device);
        std::vector<const char *> GetRequiredExtensions();
        bool CheckValidationLayerSupport();
        QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
        void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
        void HasGflwRequiredInstanceExtensions();
        bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
        SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);

        VkDebugUtilsMessengerEXT _debugMessenger;
        GLFWwindow *_window;
        VkCommandPool _commandPool;

        VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
        VkInstance _instance;
        VkDevice _device;
        VkSurfaceKHR _surface;
        VkQueue _graphicsQueue;
        VkQueue _presentQueue;

        const std::vector<const char *> _validationLayers = {"VK_LAYER_KHRONOS_validation"};
#ifdef IC_PLATFORM_MACOS
        const std::vector<const char *> _deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME,
                                                             VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
                                                             VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME};
#else
        const std::vector<const char *> _deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME,
                                                             VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME};
#endif
    };

} // namespace IC
