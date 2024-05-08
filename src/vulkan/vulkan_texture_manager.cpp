#include "vulkan_texture_manager.h"

#include "vulkan_initializers.h"
#include "vulkan_util.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace IC {
    VulkanTextureManager::VulkanTextureManager(VulkanDevice &device) : _device{device} {
        CreateImageSampler(_device.Device(), _device.properties.limits.maxSamplerAnisotropy, _defaultSampler);
    }
    VulkanTextureManager::~VulkanTextureManager() {
        for (const auto &[key, image] : _textures) {
            AllocatedImage *allocatedImage = image.get();
            vkDestroyImage(_device.Device(), allocatedImage->image, nullptr);
            vkDestroyImageView(_device.Device(), allocatedImage->view, nullptr);
            vkFreeMemory(_device.Device(), allocatedImage->memory, nullptr);
        }
        vkDestroySampler(_device.Device(), _defaultSampler, nullptr);
    }

    AllocatedImage *VulkanTextureManager::GetTexture(std::string texturePath) {
        if (_textures.contains(texturePath)) {
            return _textures[texturePath].get();
        } else {
            LoadTextureImage(texturePath);
            return _textures[texturePath].get();
        }
    }

    void VulkanTextureManager::LoadTextureImage(std::string texturePath) {
        auto texture = std::make_unique<AllocatedImage>();
        int texWidth, texHeight, texChannels;
        stbi_uc *pixels = stbi_load(texturePath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        VkDeviceSize imageSize = texWidth * texHeight * 4;

        if (!pixels) {
            IC_CORE_ERROR("Failed to load texture image.");
        }

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;

        _device.CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer,
                             stagingBufferMemory);

        void *data;
        vkMapMemory(_device.Device(), stagingBufferMemory, 0, imageSize, 0, &data);
        memcpy(data, pixels, static_cast<size_t>(imageSize));
        vkUnmapMemory(_device.Device(), stagingBufferMemory);
        stbi_image_free(pixels);

        VkCommandBuffer commandBuffer = _device.BeginSingleTimeCommands();
        CreateImage(&_device, texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
                    VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                    *texture);
        TransitionImageLayout(commandBuffer, texture->image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED,
                              VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        _device.EndSingleTimeCommands(commandBuffer);

        _device.CopyBufferToImage(stagingBuffer, texture->image, static_cast<uint32_t>(texWidth),
                                  static_cast<uint32_t>(texHeight), 1);
        commandBuffer = _device.BeginSingleTimeCommands();
        TransitionImageLayout(commandBuffer, texture->image, VK_FORMAT_R8G8B8A8_SRGB,
                              VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        _device.EndSingleTimeCommands(commandBuffer);

        vkDestroyBuffer(_device.Device(), stagingBuffer, nullptr);
        vkFreeMemory(_device.Device(), stagingBufferMemory, nullptr);

        _textures[texturePath] = std::move(texture);
    }
} // namespace IC