#pragma once

#include "ic_renderer.h"

#include "descriptors.h"
#include "pipelines.h"
#include "swap_chain.h"
#include "vulkan_initializers.h"
#include "vulkan_texture_manager.h"
#include "vulkan_types.h"

namespace IC {
    class VulkanRenderer : public Renderer {
    public:
        VulkanRenderer(const RendererConfig &config);
        virtual ~VulkanRenderer();

        void AddGameObject(std::shared_ptr<GameObject> object) override;
        void DrawFrame() override;
        static void FramebufferResizeCallback(GLFWwindow *window, int width, int height);

    private:
        void CreateCommandBuffers();
        void FreeCommandBuffers();
        void InitDescriptorAllocators();
        void RecreateSwapChain();
        void RenderImGui(VkCommandBuffer cBuffer, VkImageView targetImageView);

        // GameObject helpers
        void AddDirectionalLight(DirectionalLight *light);
        void AddPointLight(PointLight *light, glm::vec3 position);
        void AddMesh(Mesh &mesh, Transform &transform);

        // function pointers (for mac)
        PFN_vkCmdBeginRenderingKHR VulkanBeginRendering{};
        PFN_vkCmdEndRenderingKHR VulkanEndRendering{};

        // vulkan Helper Classes
        VulkanDevice _vulkanDevice;
        VulkanAllocator _allocator;
        VulkanTextureManager _textureManager;
        std::unique_ptr<SwapChain> _swapChain;
        PipelineManager _pipelineManager{};
        DescriptorAllocator _meshDescriptorAllocator{};
        DescriptorAllocator _imGuiDescriptorAllocator{};

        // rendering data (mesh, lights)
        SceneLightDescriptors _lightData{};
        std::vector<MeshRenderData> _renderData{};
        std::vector<std::shared_ptr<GameObject>> _gameObjects;

        // command buffers
        std::vector<VkCommandBuffer> _cBuffers{};

        // window information
        VkExtent2D _windowExtent;
        bool _framebufferResized = false;
    };
} // namespace IC
