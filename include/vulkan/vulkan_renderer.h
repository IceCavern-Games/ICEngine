#pragma once
#include <renderer.h>

#include <descriptors.h>
#include <pipelines.h>
#include <swap_chain.h>
#include <vulkan_types.h>

namespace IC::Renderer
{
    class VulkanRenderer : public ICRenderer
    {
    public:
        VulkanRenderer(RendererConfig &config);
        ~VulkanRenderer();

        void DrawFrame();
        void AddMesh(ICMesh &meshData, ICMaterial &materialData);

    private:
        void CreateCommandBuffers();
        void InitDescriptorAllocator();

        static ICRenderer *MakeVulkan(RendererConfig renderer_config)
        {
            return new VulkanRenderer(renderer_config);
        }

        VulkanDevice _vulkanDevice{pWindow};
        SwapChain _swapChain;
        PipelineManager _pipelineManager{};
        DescriptorAllocator _descriptorAllocator{};

        std::vector<MeshRenderData> _renderData;
        std::vector<VkCommandBuffer> _cBuffers;
    };
}