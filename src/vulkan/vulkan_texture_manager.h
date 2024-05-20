#pragma once

#include "vulkan_allocator.h"
#include "vulkan_device.h"
#include "vulkan_types.h"

#include <map>

namespace IC {
    class VulkanTextureManager {
    public:
        VulkanTextureManager(VulkanDevice &device, VulkanAllocator &allocator);
        ~VulkanTextureManager();

        AllocatedImage *GetTexture(std::string texturePath);

        VkSampler DefaultSampler() { return _defaultSampler; };

    private:
        VulkanTextureManager(const VulkanTextureManager &) = delete;
        void operator=(const VulkanTextureManager &) = delete;
        bool LoadTextureImage(std::string texturePath);

        const std::string DEFAULT_TEXTURE_PATH = "resources/textures/default_texture.png";

        VulkanAllocator &_allocator;
        VulkanDevice &_device;

        VkSampler _defaultSampler;
        std::unordered_map<std::string, std::unique_ptr<AllocatedImage>> _textures;
    };
} // namespace IC