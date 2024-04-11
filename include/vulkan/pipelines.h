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
        static VkShaderModule CreateShaderModule(VkDevice device, const std::string &filePath);

        void clear();

        VkPipeline BuildPipeline(VkDevice device);
        VkPipeline BuildComputePipeline(VkDevice device);
        void SetShaders(VkShaderModule vertexShader, VkShaderModule fragmentShader);
        void SetComputeShader(VkShaderModule computeShader);
        void SetInputTopology(VkPrimitiveTopology topology);
        void SetPolygonMode(VkPolygonMode mode);
        void SetCullMode(VkCullModeFlags cullMode, VkFrontFace frontFace);
        void SetMultisamplingNone();
        void DisableBlending();
        void EnableBlending();
        void SetColorAttachmentFormat(VkFormat format);
        void SetDepthFormat(VkFormat format);
        void DisableDepthTest();
        void EnableDepthTest();

    private:
        static VkPipelineShaderStageCreateInfo ShaderStageCreateInfo(VkShaderStageFlagBits flags,
                                                                     VkShaderModule module);
    };

    class PipelineManager
    {
    public:
        std::shared_ptr<Pipeline> FindOrCreateSuitablePipeline(VkDevice device, SwapChain &swapChain, Material &materialData);

    private:
        bool IsPipelineSuitable(Pipeline &pipeline, Material &materialData);
        std::vector<std::shared_ptr<Pipeline>> _createdPipelines;
    };
} // namespace lve
