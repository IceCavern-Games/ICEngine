#include <vulkan_initializers.h>

#include <pipelines.h>
#include <swap_chain.h>
#include <descriptors.h>
#include <vulkan_util.h>

#include <iostream>

namespace IC::Renderer::Init
{
    VkRenderingAttachmentInfo attachmentInfo(VkImageView view, VkClearValue *clear, VkImageLayout layout)
    {
        VkRenderingAttachmentInfo info = {.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO};
        info.pNext = nullptr;

        info.imageView = view;
        info.imageLayout = layout;
        info.loadOp = clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
        info.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        if (clear)
        {
            info.clearValue = *clear;
        }

        return info;
    }

    VkCommandBufferBeginInfo commandBufferBeginInfo(VkCommandBufferUsageFlags flags /*= 0*/)
    {
        VkCommandBufferBeginInfo info = {.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
        info.pNext = nullptr;
        info.pInheritanceInfo = nullptr;
        info.flags = flags;

        return info;
    }

    VkCommandBufferSubmitInfo commandBufferSubmitInfo(VkCommandBuffer cmd)
    {
        VkCommandBufferSubmitInfo info = {.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO};
        info.pNext = nullptr;
        info.commandBuffer = cmd;
        info.deviceMask = 0;

        return info;
    }

    VkRenderingInfo renderingInfo(VkExtent2D renderExtent, VkRenderingAttachmentInfo *colorAttachment, VkRenderingAttachmentInfo *depthAttachment)
    {
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

    VkSubmitInfo2 submitInfo(VkCommandBufferSubmitInfo *cmd, VkSemaphoreSubmitInfo *signalSemaphoreInfo, VkSemaphoreSubmitInfo *waitSemaphoreInfo)
    {
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

    // images
    void createImage(VulkanDevice *device, uint32_t width, uint32_t height, VkFormat format,
                     VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
                     AllocatedImage &image)
    {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = static_cast<uint32_t>(width);
        imageInfo.extent.height = static_cast<uint32_t>(height);
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.flags = 0;

        device->createImageWithInfo(imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image.image, image.memory);
        image.view = device->createImageView(image.image, format);
    }

    void createImageSampler(VkDevice device, float maxAnisotropy, VkSampler &textureSampler)
    {
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = maxAnisotropy;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 0.0f;

        if (vkCreateSampler(device, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create texture sampler!");
        }
    }

    // pipelines
    std::shared_ptr<Pipeline> createOpaquePipeline(VkDevice device, SwapChain &swapChain, ICMaterial &materialData)
    {
        // descriptor sets
        DescriptorLayoutBuilder descriptorLayoutBuilder{};
        descriptorLayoutBuilder.add_binding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER); // uniform buffer object
        descriptorLayoutBuilder.add_binding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER); // constants buffer object

        VkDescriptorSetLayout descriptorSetLayout = descriptorLayoutBuilder.build(device, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);

        // pipeline layout
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;

        VkPipelineLayout pipelineLayout;
        VK_CHECK(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout));

        PipelineBuilder pipelineBuilder;

        pipelineBuilder.pipelineLayout = pipelineLayout;
        std::cout << "before shaders\n";
        VkShaderModule vertShaderModule = PipelineBuilder::createShaderModule(device, materialData.vertShaderData);
        VkShaderModule fragShaderModule = PipelineBuilder::createShaderModule(device, materialData.fragShaderData);
        std::cout << "after shaders\n";

        pipelineBuilder.setShaders(vertShaderModule, fragShaderModule);
        pipelineBuilder.setInputTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
        pipelineBuilder.setPolygonMode(VK_POLYGON_MODE_FILL);
        pipelineBuilder.setCullMode(VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE);
        pipelineBuilder.setMultisamplingNone();
        pipelineBuilder.disableBlending();
        pipelineBuilder.enableDepthTest();
        pipelineBuilder.setColorAttachmentFormat(swapChain.getSwapChainImageFormat());
        pipelineBuilder.setDepthFormat(swapChain.getSwapChainDepthFormat());

        std::shared_ptr<Pipeline> pipeline = std::make_shared<Pipeline>();
        pipeline->pipeline = pipelineBuilder.buildPipeline(device);
        pipeline->layout = pipelineLayout;
        pipeline->descriptorSetLayout = descriptorSetLayout;
        pipeline->shaderModules = {vertShaderModule, fragShaderModule};
        pipeline->transparent = false;

        return pipeline;
    }
}