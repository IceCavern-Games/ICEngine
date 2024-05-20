#include "vulkan_renderer.h"

#include <ic_log.h>

#include "vulkan_util.h"

#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

namespace IC {
    VulkanRenderer::VulkanRenderer(const RendererConfig &config)
        : Renderer(config),
          _vulkanDevice(config.window),
          _allocator{_vulkanDevice},
          _textureManager{_vulkanDevice, _allocator},
          _windowExtent{static_cast<uint32_t>(config.width), static_cast<uint32_t>(config.height)} {
        // find rendering functions
        VulkanBeginRendering =
            (PFN_vkCmdBeginRenderingKHR)vkGetInstanceProcAddr(_vulkanDevice.Instance(), "vkCmdBeginRenderingKHR");
        VulkanEndRendering =
            (PFN_vkCmdEndRenderingKHR)vkGetInstanceProcAddr(_vulkanDevice.Instance(), "vkCmdEndRenderingKHR");

        RecreateSwapChain();

        CreateCommandBuffers();
        InitDescriptorAllocators();
        InitImGui(_vulkanDevice, window, _imGuiDescriptorAllocator.GetDescriptorPool(),
                  _swapChain->GetSwapChainImageFormat());

        // window resize callback
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, FramebufferResizeCallback);
    }

    VulkanRenderer::~VulkanRenderer() {
        glfwSetWindowUserPointer(window, nullptr);
        glfwSetFramebufferSizeCallback(window, nullptr);

        ImGui_ImplVulkan_Shutdown();
        _meshDescriptorAllocator.DestroyDescriptorPool(_vulkanDevice.Device());
        _imGuiDescriptorAllocator.DestroyDescriptorPool(_vulkanDevice.Device());
        _pipelineManager.DestroyPipelines(_vulkanDevice.Device());

        for (auto mesh : _renderData) {
            _allocator.DestroyBuffer(mesh.vertexBuffer);
            _allocator.DestroyBuffer(mesh.indexBuffer);

            for (size_t i = 0; i < SwapChain::MAX_FRAMES_IN_FLIGHT; i++) {
                _allocator.DestroyBuffer(mesh.mvpBuffers[i]);
                _allocator.DestroyBuffer(mesh.materialBuffers[i]);
            }
            for (auto buffer : mesh.lightsBuffers) {
                _allocator.DestroyBuffer(buffer);
            }
        }
    }

    void VulkanRenderer::CreateCommandBuffers() {
        _cBuffers.resize(_swapChain->ImageCount());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = _vulkanDevice.GetCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(_cBuffers.size());

        VK_CHECK(vkAllocateCommandBuffers(_vulkanDevice.Device(), &allocInfo, _cBuffers.data()));
    }

    void VulkanRenderer::FreeCommandBuffers() {
        vkFreeCommandBuffers(_vulkanDevice.Device(), _vulkanDevice.GetCommandPool(),
                             static_cast<uint32_t>(_cBuffers.size()), _cBuffers.data());
        _cBuffers.clear();
    }

    void VulkanRenderer::DrawFrame() {
        double start = glfwGetTime();

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        for (auto keyValue : imGuiFunctions) {
            keyValue.second();
        }
        ImGui::Render();

        renderStats.drawCalls = 0;
        renderStats.numTris = 0;
        renderStats.frametime = 0.0f;

        static float rotation = 0.0f;
        rotation += 0.01f;

        uint32_t imageIndex;
        auto result = _swapChain->AcquireNextImage(&imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            RecreateSwapChain();
            return;
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            IC_CORE_ERROR("Failed to acquire swap chain image.");
            throw std::runtime_error("Failed to acquire swap chain image.");
        }

        _swapChain->WaitForFrameFence(&imageIndex);

        vkResetCommandBuffer(_cBuffers[imageIndex], 0);

        VkCommandBufferBeginInfo beginInfo = CommandBufferBeginInfo();

        if (vkBeginCommandBuffer(_cBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
            IC_CORE_ERROR("Failed to begin recording command buffer.");
            throw std::runtime_error("Failed to begin recording command buffer.");
        }

        VkRenderingAttachmentInfo colorAttachment = {.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO};
        colorAttachment.imageView = _swapChain->GetImageView(imageIndex);
        colorAttachment.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
        colorAttachment.clearValue = {0.1f, 0.1f, 0.1f, 1.0f};
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

        VkRenderingAttachmentInfo depthAttachment{.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO};
        depthAttachment.imageView = _swapChain->GetDepthImage(imageIndex).view;
        depthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
        depthAttachment.clearValue.depthStencil = {1.0f, 0};
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

        VkRenderingInfo renderingInfo{.sType = VK_STRUCTURE_TYPE_RENDERING_INFO};
        renderingInfo.renderArea = VkRect2D{VkOffset2D{0, 0}, _swapChain->GetSwapChainExtent()};
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = 1;
        renderingInfo.pColorAttachments = &colorAttachment;
        renderingInfo.pDepthAttachment = &depthAttachment;
        renderingInfo.pStencilAttachment = nullptr;

        VkViewport viewport = {};
        viewport.x = 0;
        viewport.y = 0;
        viewport.width = _swapChain->GetSwapChainExtent().width;
        viewport.height = _swapChain->GetSwapChainExtent().height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        vkCmdSetViewport(_cBuffers[imageIndex], 0, 1, &viewport);

        VkRect2D scissor = {};
        scissor.offset.x = 0;
        scissor.offset.y = 0;
        scissor.extent.width = _swapChain->GetSwapChainExtent().width;
        scissor.extent.height = _swapChain->GetSwapChainExtent().height;

        vkCmdSetScissor(_cBuffers[imageIndex], 0, 1, &scissor);

        TransitionImageLayout(_cBuffers[imageIndex], _swapChain->GetImage(imageIndex),
                              _swapChain->GetSwapChainImageFormat(), VK_IMAGE_LAYOUT_UNDEFINED,
                              VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

        VulkanBeginRendering(_cBuffers[imageIndex], &renderingInfo);
        for (MeshRenderData data : _renderData) {
            // bind pipeline todo: only bind if different
            vkCmdBindPipeline(_cBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, data.renderPipeline->pipeline);

            TransformationPushConstants pushConstants{};
            pushConstants.model = glm::translate(glm::mat4(1.0f), data.transform.Position()) *
                                  glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), {1.0f, 0.0f, 0.0f}) *
                                  glm::rotate(glm::mat4(1.0f), rotation, {0.0f, 1.0f, 0.0f}) *
                                  glm::scale(glm::mat4(1.0f), data.transform.Scale());
            pushConstants.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

            vkCmdPushConstants(_cBuffers[imageIndex], data.renderPipeline->layout,
                               VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                               sizeof(TransformationPushConstants), &pushConstants);

            if (data.meshData.Material()->Template().flags & MaterialFlags::Lit) {
                // update light data
                data.UpdateUniformBuffer<SceneLightDescriptors>(_lightData,
                                                                data.lightsBuffers[_swapChain->GetCurrentFrame()]);
            }

            data.Bind(_cBuffers[imageIndex], data.renderPipeline->layout, _swapChain->GetCurrentFrame());
            data.Draw(_cBuffers[imageIndex]);
            renderStats.drawCalls++;
            renderStats.numTris += data.meshData.IndexCount() / 3;
        }

        VulkanEndRendering(_cBuffers[imageIndex]);

        RenderImGui(_cBuffers[imageIndex], _swapChain->GetImageView(imageIndex));

        TransitionImageLayout(_cBuffers[imageIndex], _swapChain->GetImage(imageIndex),
                              _swapChain->GetSwapChainImageFormat(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                              VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

        if (vkEndCommandBuffer(_cBuffers[imageIndex]) != VK_SUCCESS) {
            IC_CORE_ERROR("Failed to record command buffer.");
            throw std::runtime_error("Failed to record command buffer.");
        }

        result = _swapChain->SubmitCommandBuffers(&_cBuffers[imageIndex], &imageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || _framebufferResized) {
            _framebufferResized = false;
            RecreateSwapChain();
            return;
        }

        if (result != VK_SUCCESS) {
            IC_CORE_ERROR("Failed to present swap chain image.");
            throw std::runtime_error("Failed to present swap chain image.");
        }

        vkDeviceWaitIdle(_vulkanDevice.Device());

        double end = glfwGetTime();
        double elapsed = end - start;
        renderStats.frametime = elapsed * 1000.0f;
    }

    void VulkanRenderer::FramebufferResizeCallback(GLFWwindow *window, int width, int height) {
        auto renderer = reinterpret_cast<VulkanRenderer *>(glfwGetWindowUserPointer(window));
        renderer->_framebufferResized = true;
        renderer->_windowExtent.width = width;
        renderer->_windowExtent.height = height;
    }

    void VulkanRenderer::InitDescriptorAllocators() {
        // mesh descriptor pool
        std::vector<VkDescriptorPoolSize> poolSizes{};
        poolSizes.push_back({VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000});
        poolSizes.push_back({VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000});

        _meshDescriptorAllocator.CreateDescriptorPool(_vulkanDevice.Device(), poolSizes, 1000);

        // imgui descriptor pool
        std::vector<VkDescriptorPoolSize> guiPoolSizes{};
        guiPoolSizes.push_back({VK_DESCRIPTOR_TYPE_SAMPLER, 1000});
        guiPoolSizes.push_back({VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000});
        guiPoolSizes.push_back({VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000});
        guiPoolSizes.push_back({VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000});
        guiPoolSizes.push_back({VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000});
        guiPoolSizes.push_back({VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000});
        guiPoolSizes.push_back({VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000});
        guiPoolSizes.push_back({VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000});
        guiPoolSizes.push_back({VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000});
        guiPoolSizes.push_back({VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000});
        guiPoolSizes.push_back({VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000});

        _imGuiDescriptorAllocator.CreateDescriptorPool(_vulkanDevice.Device(), guiPoolSizes, 1000,
                                                       VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT);
    }

    // Adds a mesh and associated material to list of renderable objects
    void VulkanRenderer::AddMesh(Mesh &mesh, Transform &transform) {
        MeshRenderData meshRenderData{.meshData = mesh, .transform = transform};

        meshRenderData.renderPipeline =
            _pipelineManager.FindOrCreateSuitablePipeline(_vulkanDevice.Device(), *_swapChain.get(), *mesh.Material());

        // vertex buffers
        _allocator.CreateBuffer(mesh.Vertices().data(), sizeof(mesh.Vertices()[0]) * mesh.VertexCount(),
                                VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_AUTO, meshRenderData.vertexBuffer);
        _allocator.CreateBuffer(mesh.Indices().data(), sizeof(mesh.Indices()[0]) * mesh.IndexCount(),
                                VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_AUTO, meshRenderData.indexBuffer);

        // write descriptor sets
        _meshDescriptorAllocator.AllocateDescriptorSets(
            _vulkanDevice.Device(), meshRenderData.renderPipeline->descriptorSetLayouts, meshRenderData.descriptorSets);

        // per object descriptors (set 0)
        DescriptorWriter writer{};
        WritePerObjectDescriptors(_allocator, *_swapChain.get(), writer, meshRenderData);
        for (size_t i = 0; i < SwapChain::MAX_FRAMES_IN_FLIGHT; i++) {
            writer.UpdateSet(_vulkanDevice.Device(), meshRenderData.descriptorSets[i][0]);
        }
        writer.Clear();

        // scene data/lighting descriptors (set 1)
        if (mesh.Material()->Template().flags & MaterialFlags::Lit) {
            WriteLightDescriptors(_allocator, SwapChain::MAX_FRAMES_IN_FLIGHT, _lightData, writer,
                                  meshRenderData.lightsBuffers);

            for (size_t i = 0; i < SwapChain::MAX_FRAMES_IN_FLIGHT; i++) {
                writer.UpdateSet(_vulkanDevice.Device(), meshRenderData.descriptorSets[i][1]);
            }
            writer.Clear();
        }

        // material descriptors (set 1 or 2)
        WriteMaterialDescriptors(_allocator, SwapChain::MAX_FRAMES_IN_FLIGHT, writer, *mesh.Material(), _textureManager,
                                 meshRenderData.materialBuffers);
        for (size_t i = 0; i < SwapChain::MAX_FRAMES_IN_FLIGHT; i++) {
            int index = mesh.Material()->Template().flags & MaterialFlags::Lit ? 2 : 1;
            writer.UpdateSet(_vulkanDevice.Device(), meshRenderData.descriptorSets[i][index]);
            meshRenderData.UpdateMaterialBuffer(i);
        }

        _renderData.push_back(meshRenderData);
    }

    void VulkanRenderer::AddDirectionalLight(DirectionalLight *light) {
        _lightData.directionalLight = CreateDirectionalLightDescriptors(
            light, glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    }

    void VulkanRenderer::AddPointLight(PointLight *light, glm::vec3 position) {
        if (_lightData.numPointLights < MAX_POINT_LIGHTS) {
            _lightData.pointLights[_lightData.numPointLights] = CreatePointLightDescriptors(
                light, position,
                glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
            _lightData.numPointLights++;
        }
    }

    void VulkanRenderer::AddGameObject(std::shared_ptr<GameObject> object) {
        for (auto &component : object->Components()) {
            if (typeid(*component) == typeid(Mesh)) {
                AddMesh(*static_cast<Mesh *>(component.get()), *object->GetTransform());
            } else if (typeid(*component) == typeid(PointLight)) {
                AddPointLight(static_cast<PointLight *>(component.get()), object->GetTransform()->Position());
            } else if (typeid(*component) == typeid(DirectionalLight)) {
                AddDirectionalLight(static_cast<DirectionalLight *>(component.get()));
            }
        }
    }

    void VulkanRenderer::RecreateSwapChain() {
        while (_windowExtent.width == 0 || _windowExtent.height == 0) {
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(_vulkanDevice.Device());
        if (_swapChain == nullptr) {
            _swapChain = std::make_unique<SwapChain>(_vulkanDevice, _allocator, _windowExtent);
        } else {
            _swapChain = std::make_unique<SwapChain>(_vulkanDevice, _allocator, _windowExtent, std::move(_swapChain));
            if (_swapChain->ImageCount() != _cBuffers.size()) {
                FreeCommandBuffers();
                CreateCommandBuffers();
            }
        }
    }

    void VulkanRenderer::RenderImGui(VkCommandBuffer cBuffer, VkImageView targetImageView) {
        VkRenderingAttachmentInfo colorAttachment = AttachmentInfo(targetImageView, nullptr, VK_IMAGE_LAYOUT_GENERAL);
        VkRenderingInfo renderInfo = RenderingInfo(_swapChain->GetSwapChainExtent(), &colorAttachment, nullptr);

        VulkanBeginRendering(cBuffer, &renderInfo);

        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cBuffer);

        VulkanEndRendering(cBuffer);
    }
} // namespace IC
