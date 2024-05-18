#include "vulkan_texture_manager.h"

#include "vulkan_initializers.h"
#include "vulkan_util.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace IC {
    VulkanTextureManager::VulkanTextureManager(VulkanDevice &device, VulkanAllocator &allocator)
        : _device{device}, _allocator{allocator} {
        CreateImageSampler(_device.Device(), _device.properties.limits.maxSamplerAnisotropy, _defaultSampler);

        // load default image into manager
        LoadTextureImage(DEFAULT_TEXTURE_PATH);
    }
    VulkanTextureManager::~VulkanTextureManager() {
        for (const auto &[key, image] : _textures) {
            AllocatedImage *allocatedImage = image.get();
            _allocator.DestroyImage(*allocatedImage);
        }
        vkDestroySampler(_device.Device(), _defaultSampler, nullptr);
    }

    AllocatedImage *VulkanTextureManager::GetTexture(std::string texturePath) {
        if (texturePath.empty()) {
            return _textures[DEFAULT_TEXTURE_PATH].get();
        }

        if (!_textures.contains(texturePath)) {
            if (!LoadTextureImage(texturePath)) {
                return _textures[DEFAULT_TEXTURE_PATH].get();
            }
        }
        return _textures[texturePath].get();
    }

    bool VulkanTextureManager::LoadTextureImage(std::string texturePath) {
        stbi_set_flip_vertically_on_load(true);
        auto texture = std::make_unique<AllocatedImage>();
        int texWidth, texHeight, texChannels;
        stbi_uc *pixels = stbi_load(texturePath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        VkDeviceSize imageSize = texWidth * texHeight * 4;
        VkExtent3D size = {static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), 1};

        if (!pixels) {
            IC_CORE_ERROR("Failed to load texture image.");
            return false;
        }

        AllocatedBuffer stagingBuffer{};
        _allocator.CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_AUTO, stagingBuffer);

        memcpy(stagingBuffer.allocInfo.pMappedData, pixels, static_cast<size_t>(imageSize));
        stbi_image_free(pixels);

        VkCommandBuffer commandBuffer = _device.BeginSingleTimeCommands();

        _allocator.CreateImage(size, VK_FORMAT_R8G8B8A8_SRGB,
                               VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, *texture);
        TransitionImageLayout(commandBuffer, texture->image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED,
                              VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        _device.EndSingleTimeCommands(commandBuffer);

        _device.CopyBufferToImage(stagingBuffer.buffer, texture->image, static_cast<uint32_t>(texWidth),
                                  static_cast<uint32_t>(texHeight), 1);
        commandBuffer = _device.BeginSingleTimeCommands();
        TransitionImageLayout(commandBuffer, texture->image, VK_FORMAT_R8G8B8A8_SRGB,
                              VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        _device.EndSingleTimeCommands(commandBuffer);

        _allocator.DestroyBuffer(stagingBuffer);

        _textures[texturePath] = std::move(texture);
        return true;
    }
} // namespace IC