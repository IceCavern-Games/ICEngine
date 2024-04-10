#pragma once

#include <vulkan_device.h>
#include <vulkan_types.h>
#include <swap_chain.h>

#include <memory>

namespace IC::Renderer
{
    class PipelineBuilder
    {
    public:
        std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

        VkPipelineInputAssemblyStateCreateInfo inputAssembly;
        VkPipelineRasterizationStateCreateInfo rasterizer;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineMultisampleStateCreateInfo multisampling;
        VkPipelineLayout pipelineLayout;
        VkPipelineDepthStencilStateCreateInfo depthStencil;
        VkPipelineRenderingCreateInfo renderInfo;
        VkFormat colorAttachmentformat;

        PipelineBuilder() { clear(); }
        static VkShaderModule createShaderModule(VkDevice device, const std::string &filePath);

        void clear();

        VkPipeline buildPipeline(VkDevice device);
        VkPipeline buildComputePipeline(VkDevice device);
        void setShaders(VkShaderModule vertexShader, VkShaderModule fragmentShader);
        void setComputeShader(VkShaderModule computeShader);
        void setInputTopology(VkPrimitiveTopology topology);
        void setPolygonMode(VkPolygonMode mode);
        void setCullMode(VkCullModeFlags cullMode, VkFrontFace frontFace);
        void setMultisamplingNone();
        void disableBlending();
        void enableBlending();
        void setColorAttachmentFormat(VkFormat format);
        void setDepthFormat(VkFormat format);
        void disableDepthTest();
        void enableDepthTest();

    private:
        static VkPipelineShaderStageCreateInfo shaderStageCreateInfo(VkShaderStageFlagBits flags,
                                                                     VkShaderModule module);
    };

    class PipelineManager
    {
    public:
        std::shared_ptr<Pipeline> findOrCreateSuitablePipeline(VkDevice device, SwapChain &swapChain, ICMaterial &materialData);

    private:
        bool isPipelineSuitable(Pipeline &pipeline, ICMaterial &materialData);
        std::vector<std::shared_ptr<Pipeline>> createdPipelines;
    };
} // namespace lve