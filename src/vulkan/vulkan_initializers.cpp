#include "vulkan_initializers.h"

#include <ic_log.h>

#include "descriptors.h"
#include "pipelines.h"
#include "swap_chain.h"
#include "vulkan_util.h"

#include <iostream>

namespace IC {
    VkRenderingAttachmentInfo AttachmentInfo(VkImageView view, VkClearValue *clear, VkImageLayout layout) {
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

    VkCommandBufferBeginInfo CommandBufferBeginInfo(VkCommandBufferUsageFlags flags /*= 0*/) {
        VkCommandBufferBeginInfo info = {.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
        info.pNext = nullptr;
        info.pInheritanceInfo = nullptr;
        info.flags = flags;

        return info;
    }

    VkCommandBufferSubmitInfo CommandBufferSubmitInfo(VkCommandBuffer cmd) {
        VkCommandBufferSubmitInfo info = {.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO};
        info.pNext = nullptr;
        info.commandBuffer = cmd;
        info.deviceMask = 0;

        return info;
    }

    VkRenderingInfo RenderingInfo(VkExtent2D renderExtent, VkRenderingAttachmentInfo *colorAttachment,
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

    VkSubmitInfo2 SubmitInfo(VkCommandBufferSubmitInfo *cmd, VkSemaphoreSubmitInfo *signalSemaphoreInfo,
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

    // descriptors
    void WriteCommonDescriptors(VulkanDevice &device, SwapChain &swapChain, DescriptorWriter &writer,
                                MeshRenderData &renderData) {
        size_t maxFrames = SwapChain::MAX_FRAMES_IN_FLIGHT;
        renderData.mvpBuffers.resize(maxFrames);
        renderData.constantsBuffers.resize(maxFrames);

        for (size_t i = 0; i < maxFrames; i++) {
            // hard coded for now
            CameraDescriptors projection{};
            projection.proj = glm::perspective(
                glm::radians(45.0f),
                (float)swapChain.GetSwapChainExtent().width / swapChain.GetSwapChainExtent().height, 0.1f, 10.0f);

            CreateAndFillBuffer(device, &projection, sizeof(CameraDescriptors), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                renderData.mvpBuffers[i]);

            CreateAndFillBuffer(device, &renderData.materialData.constants, sizeof(MaterialConstants),
                                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                                renderData.constantsBuffers[i]);

            vkMapMemory(device.Device(), renderData.mvpBuffers[i].memory, 0, sizeof(CameraDescriptors), 0,
                        &renderData.mvpBuffers[i].mappedMemory);
            vkMapMemory(device.Device(), renderData.constantsBuffers[i].memory, 0, sizeof(MaterialConstants), 0,
                        &renderData.constantsBuffers[i].mappedMemory);

            writer.WriteBuffer(0, renderData.mvpBuffers[i].buffer, sizeof(CameraDescriptors), 0,
                               VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
            writer.WriteBuffer(1, renderData.constantsBuffers[i].buffer, sizeof(MaterialConstants), 0,
                               VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
        }
    }

    void WriteLightDescriptors(VulkanDevice &device, size_t maxFrames, SceneLightDescriptors &lightData,
                               DescriptorWriter &writer, std::vector<AllocatedBuffer> &lightBuffers) {
        lightBuffers.resize(maxFrames);
        for (size_t i = 0; i < maxFrames; i++) {
            CreateAndFillBuffer(device, &lightData, sizeof(SceneLightDescriptors), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, lightBuffers[i]);
            vkMapMemory(device.Device(), lightBuffers[i].memory, 0, sizeof(SceneLightDescriptors), 0,
                        &lightBuffers[i].mappedMemory);

            writer.WriteBuffer(2, lightBuffers[i].buffer, sizeof(SceneLightDescriptors), 0,
                               VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
        }
    }

    // images
    void CreateImage(VulkanDevice *device, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
                     VkImageUsageFlags usage, VkMemoryPropertyFlags properties, AllocatedImage &image) {
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

        device->CreateImageWithInfo(imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image.image, image.memory);
        image.view = device->CreateImageView(image.image, format);
    }

    void CreateImageSampler(VkDevice device, float maxAnisotropy, VkSampler &textureSampler) {
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

        if (vkCreateSampler(device, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
            IC_CORE_ERROR("Failed to create texture sampler.");
            throw std::runtime_error("Failed to create texture sampler.");
        }
    }

    // pipelines
    std::shared_ptr<Pipeline> CreateOpaquePipeline(VkDevice device, SwapChain &swapChain, Material &materialData) {
        // descriptor sets
        DescriptorLayoutBuilder descriptorLayoutBuilder{};
        descriptorLayoutBuilder.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER); // uniform buffer object
        descriptorLayoutBuilder.AddBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER); // constants buffer object

        if (materialData.flags & MaterialFlags::Lit) {
            descriptorLayoutBuilder.AddBinding(2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
        }

        VkDescriptorSetLayout descriptorSetLayout =
            descriptorLayoutBuilder.Build(device, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);

        // pipeline layout
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;

        // push constants
        VkPushConstantRange pushConstants = PushConstants<TransformationPushConstants>(VK_SHADER_STAGE_VERTEX_BIT);
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstants;

        VkPipelineLayout pipelineLayout;
        VK_CHECK(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout));

        PipelineBuilder pipelineBuilder;

        pipelineBuilder.pipelineLayout = pipelineLayout;
        VkShaderModule vertShaderModule = PipelineBuilder::CreateShaderModule(device, materialData.vertShaderData);
        VkShaderModule fragShaderModule = PipelineBuilder::CreateShaderModule(device, materialData.fragShaderData);

        pipelineBuilder.SetShaders(vertShaderModule, fragShaderModule);
        pipelineBuilder.SetInputTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
        pipelineBuilder.SetPolygonMode(VK_POLYGON_MODE_FILL);
        pipelineBuilder.SetCullMode(VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_CLOCKWISE);
        pipelineBuilder.SetMultisamplingNone();
        materialData.flags &MaterialFlags::Transparent ? pipelineBuilder.EnableBlending()
                                                       : pipelineBuilder.DisableBlending();
        pipelineBuilder.EnableDepthTest();
        pipelineBuilder.SetColorAttachmentFormat(swapChain.GetSwapChainImageFormat());
        pipelineBuilder.SetDepthFormat(swapChain.GetSwapChainDepthFormat());

        std::shared_ptr<Pipeline> pipeline = std::make_shared<Pipeline>();
        pipeline->pipeline = pipelineBuilder.BuildPipeline(device);
        pipeline->layout = pipelineLayout;
        pipeline->descriptorSetLayout = descriptorSetLayout;
        pipeline->shaderModules = {vertShaderModule, fragShaderModule};
        pipeline->materialFlags = materialData.flags;

        return pipeline;
    }

    // ImGui
    void InitImGui(VulkanDevice &device, GLFWwindow *window, VkDescriptorPool descriptorPool, VkFormat imageFormat) {
        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForVulkan(window, true);

        ImGui_ImplVulkan_InitInfo initInfo = {};
        initInfo.Instance = device.Instance();
        initInfo.PhysicalDevice = device.PhysicalDevice();
        initInfo.Device = device.Device();
        initInfo.Queue = device.GraphicsQueue();
        initInfo.DescriptorPool = descriptorPool;
        initInfo.MinImageCount = 3;
        initInfo.ImageCount = 3;
        initInfo.UseDynamicRendering = true;
        initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        initInfo.ColorAttachmentFormat = imageFormat;

        ImGui_ImplVulkan_Init(&initInfo, VK_NULL_HANDLE);
    }
} // namespace IC
