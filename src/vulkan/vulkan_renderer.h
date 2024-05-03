#pragma once

#include "ic_renderer.h"

#include "descriptors.h"
#include "pipelines.h"
#include "swap_chain.h"
#include "vulkan_initializers.h"
#include "vulkan_types.h"

namespace IC {
    class VulkanRenderer : public Renderer {
    public:
        VulkanRenderer(const RendererConfig &config);
        virtual ~VulkanRenderer();

        void AddMesh(Mesh &meshData, Material &materialData) override;
        void AddLight(std::shared_ptr<PointLight> light) override;
        void AddDirectionalLight(std::shared_ptr<DirectionalLight> light) override;
        void DrawFrame() override;
        static void FramebufferResizeCallback(GLFWwindow *window, int width, int height);

    private:
        void CreateCommandBuffers();
        void FreeCommandBuffers();
        void InitDescriptorAllocators();
        void RecreateSwapChain();
        void RenderImGui(VkCommandBuffer cBuffer, VkImageView targetImageView);

        // function pointers (for mac)
        PFN_vkCmdBeginRenderingKHR VulkanBeginRendering{};
        PFN_vkCmdEndRenderingKHR VulkanEndRendering{};

        // vulkan Helper Classes
        VulkanDevice _vulkanDevice;
        std::unique_ptr<SwapChain> _swapChain;
        PipelineManager _pipelineManager{};
        DescriptorAllocator _meshDescriptorAllocator{};
        DescriptorAllocator _imGuiDescriptorAllocator{};

        // rendering data (mesh, lights)
        std::vector<MeshRenderData> _renderData{};
        std::vector<std::shared_ptr<PointLight>> _pointLights;
        std::shared_ptr<DirectionalLight> _directionalLight;

        // command buffers
        std::vector<VkCommandBuffer> _cBuffers{};

        // window information
        VkExtent2D _windowExtent;
        bool _framebufferResized = false;
    };
} // namespace IC
