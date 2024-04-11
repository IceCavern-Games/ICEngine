#include <vulkan_renderer.h>

#include <vulkan_initializers.h>
#include <vulkan_util.h>

#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

namespace IC::Renderer
{
    VulkanRenderer::VulkanRenderer(RendererConfig &config) : ICRenderer{config}, swapChain{vulkanDevice, {static_cast<uint32_t>(config.width), static_cast<uint32_t>(config.height)}}
    {
        create_command_buffers();
        init_descriptor_allocator();
    }

    VulkanRenderer::~VulkanRenderer()
    {
        descriptorAllocator.destroyDescriptorPool(vulkanDevice.device());
    }

    void VulkanRenderer::create_command_buffers()
    {
        cBuffers.resize(swapChain.imageCount());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = vulkanDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(cBuffers.size());

        VK_CHECK(vkAllocateCommandBuffers(vulkanDevice.device(), &allocInfo, cBuffers.data()));
    }

    void VulkanRenderer::draw_frame()
    {
        uint32_t imageIndex;
        auto result = swapChain.acquireNextImage(&imageIndex);

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("failed to acquire swap chain image");
        }

        swapChain.waitForFrameFence(&imageIndex);

        vkResetCommandBuffer(cBuffers[imageIndex], 0);

        VkCommandBufferBeginInfo beginInfo = Init::commandBufferBeginInfo();

        if (vkBeginCommandBuffer(cBuffers[imageIndex], &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to begin recording command buffer");
        }

        VkRenderingAttachmentInfo colorAttachment = {.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO};
        colorAttachment.imageView = swapChain.getImageView(imageIndex);
        colorAttachment.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
        colorAttachment.clearValue = {0.1f, 0.1f, 0.1f, 1.0f};
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

        VkRenderingAttachmentInfo depthAttachment{.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO};
        depthAttachment.imageView = swapChain.getDepthImageView(imageIndex);
        depthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
        depthAttachment.clearValue.depthStencil = {1.0f, 0};
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

        VkRenderingInfo renderingInfo{.sType = VK_STRUCTURE_TYPE_RENDERING_INFO};
        renderingInfo.renderArea = VkRect2D{VkOffset2D{0, 0}, swapChain.getSwapChainExtent()};
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = 1;
        renderingInfo.pColorAttachments = &colorAttachment;
        renderingInfo.pDepthAttachment = &depthAttachment;
        renderingInfo.pStencilAttachment = nullptr;

        auto function = vkGetInstanceProcAddr(vulkanDevice.instance(), "vkCmdBeginRenderingKHR");
        ((PFN_vkCmdBeginRenderingKHR)function)(cBuffers[imageIndex], &renderingInfo);

        VkViewport viewport = {};
        viewport.x = 0;
        viewport.y = 0;
        viewport.width = swapChain.getSwapChainExtent().width;
        viewport.height = swapChain.getSwapChainExtent().height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        vkCmdSetViewport(cBuffers[imageIndex], 0, 1, &viewport);

        VkRect2D scissor = {};
        scissor.offset.x = 0;
        scissor.offset.y = 0;
        scissor.extent.width = swapChain.getSwapChainExtent().width;
        scissor.extent.height = swapChain.getSwapChainExtent().height;

        vkCmdSetScissor(cBuffers[imageIndex], 0, 1, &scissor);

        for (MeshRenderData data : renderData)
        {
            // bind pipeline todo: only bind if different
            vkCmdBindPipeline(cBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, data.renderPipeline->pipeline);

            MVPObject ubo{};
            // hard coded for now
            ubo.model = glm::scale(glm::mat4(1.0f), {0.2f, 0.2f, 0.2f});
            ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            ubo.proj = glm::perspective(glm::radians(45.0f), (float)swapChain.getSwapChainExtent().width / swapChain.getSwapChainExtent().height, 0.1f, 10.0f);

            data.update_mvp_buffer(ubo, swapChain.getCurrentFrame());
            data.bind(cBuffers[imageIndex], data.renderPipeline->layout, swapChain.getCurrentFrame());
            data.draw(cBuffers[imageIndex]);
        }

        auto function2 = vkGetInstanceProcAddr(vulkanDevice.instance(), "vkCmdEndRenderingKHR");
        ((PFN_vkCmdEndRenderingKHR)function2)(cBuffers[imageIndex]);

        // transitioning to COLOR_ATTACHMENT_OPTIMAL for imgui, imgui expects image to be in this format
        // todo: actually implement imgui
        Util::transition_image_layout(cBuffers[imageIndex], swapChain.getImage(imageIndex), swapChain.getSwapChainImageFormat(), VK_IMAGE_LAYOUT_UNDEFINED,
                                      VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

        Util::transition_image_layout(cBuffers[imageIndex], swapChain.getImage(imageIndex), swapChain.getSwapChainImageFormat(),
                                      VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

        if (vkEndCommandBuffer(cBuffers[imageIndex]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer");
        }

        result = swapChain.submitCommandBuffers(&cBuffers[imageIndex], &imageIndex);
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to present swap chain image");
        }
    }

    void VulkanRenderer::init_descriptor_allocator()
    {
        std::vector<VkDescriptorPoolSize> poolSizes{};
        poolSizes.push_back({VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, static_cast<uint32_t>(SwapChain::MAX_FRAMES_IN_FLIGHT)});
        poolSizes.push_back({VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, static_cast<uint32_t>(SwapChain::MAX_FRAMES_IN_FLIGHT)});

        descriptorAllocator.createDescriptorPool(vulkanDevice.device(), poolSizes, 100);
    }

    // Adds a mesh and associated material to list of renderable objects
    void VulkanRenderer::add_mesh(ICMesh &meshData, ICMaterial &materialData)
    {
        MeshRenderData meshRenderData{
            .meshData = meshData,
            .materialData = materialData};

        meshRenderData.renderPipeline = pipelineManager.findOrCreateSuitablePipeline(vulkanDevice.device(), swapChain, materialData);

        // buffers
        Util::create_and_fill_buffer(vulkanDevice, meshData.vertices.data(), sizeof(meshData.vertices[0]) * meshData.vertex_count, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, meshRenderData.vertexBuffer);
        Util::create_and_fill_buffer(vulkanDevice, meshData.indices.data(), sizeof(meshData.indices[0]) * meshData.index_count, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, meshRenderData.indexBuffer);

        meshRenderData.mvp_buffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
        meshRenderData.constants_buffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
        for (size_t i = 0; i < SwapChain::MAX_FRAMES_IN_FLIGHT; i++)
        {
            vulkanDevice.createBuffer(sizeof(MVPObject), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                      meshRenderData.mvp_buffers[i].buffer, meshRenderData.mvp_buffers[i].memory);

            Util::create_and_fill_buffer(vulkanDevice, &materialData.constants, sizeof(MaterialConstants), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, meshRenderData.constants_buffers[i]);

            vkMapMemory(vulkanDevice.device(), meshRenderData.mvp_buffers[i].memory, 0, sizeof(MVPObject), 0, &meshRenderData.mvp_buffers[i].mapped_memory);
            vkMapMemory(vulkanDevice.device(), meshRenderData.constants_buffers[i].memory, 0, sizeof(MaterialConstants), 0, &meshRenderData.constants_buffers[i].mapped_memory);
        }

        // write descriptor sets
        descriptorAllocator.allocateDescriptorSets(vulkanDevice.device(), meshRenderData.renderPipeline->descriptorSetLayout, meshRenderData.descriptorSets);
        DescriptorWriter writer{};
        for (size_t i = 0; i < SwapChain::MAX_FRAMES_IN_FLIGHT; i++)
        {
            writer.write_buffer(0, meshRenderData.mvp_buffers[i].buffer, sizeof(MVPObject), 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
            writer.write_buffer(1, meshRenderData.constants_buffers[i].buffer, sizeof(MaterialConstants), 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);

            writer.update_set(vulkanDevice.device(), meshRenderData.descriptorSets[i]);
        }

        renderData.push_back(meshRenderData);
    }
}