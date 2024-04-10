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

        void draw_frame();
        void add_mesh(ICMesh &meshData, ICMaterial &materialData);

    private:
        void create_command_buffers();
        void init_descriptor_allocator();

        static ICRenderer *make_vulkan(RendererConfig renderer_config)
        {
            return new VulkanRenderer(renderer_config);
        }

        VulkanDevice vulkanDevice{pWindow};
        SwapChain swapChain;
        PipelineManager pipelineManager{};
        DescriptorAllocator descriptorAllocator{};

        std::vector<MeshRenderData> renderData;
        std::vector<VkCommandBuffer> cBuffers;
    };
}