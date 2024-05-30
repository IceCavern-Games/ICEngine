#include "pipelines.h"

#include <ic_log.h>

#include "swap_chain.h"
#include "vulkan_initializers.h"
#include "vulkan_types.h"
#include "vulkan_util.h"

#include <fstream>
#include <iostream>
#include <stdexcept>

namespace IC {
    void PipelineBuilder::clear() {
        inputAssembly = {.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
        rasterizer = {.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
        colorBlendAttachment = {};
        multisampling = {.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
        pipelineLayout = {};
        depthStencil = {.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};
        renderInfo = {.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR};
        shaderStages.clear();
    }

    VkShaderModule PipelineBuilder::CreateShaderModule(VkDevice device, const std::string &filePath) {
        std::ifstream file{filePath, std::ios::ate | std::ios::binary};

        if (!file.is_open()) {
            IC_CORE_ERROR("Failed to open file {0}.", filePath);
            throw std::runtime_error("Failed to open file " + filePath + ".");
        }

        size_t fileSize = static_cast<size_t>(file.tellg());
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();

        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = buffer.size();
        createInfo.pCode = reinterpret_cast<const uint32_t *>(buffer.data());

        VkShaderModule shaderModule;

        if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
            IC_CORE_ERROR("Failed to create shader module.");
            throw std::runtime_error("Failed to create shader module.");
        }

        return shaderModule;
    }

    VkPipeline PipelineBuilder::BuildPipeline(VkDevice device) {
        VkPipelineViewportStateCreateInfo viewportState = {};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.pNext = nullptr;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        VkPipelineColorBlendStateCreateInfo colorBlending = {};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.pNext = nullptr;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;

        auto bindingDescription = GetVertexBindingDescription();
        auto attributeDescriptions = GetVertexAttributeDescriptions();
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;

        VkGraphicsPipelineCreateInfo pipelineInfo = {.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
        pipelineInfo.pNext = &renderInfo;

        pipelineInfo.stageCount = (uint32_t)shaderStages.size();
        pipelineInfo.pStages = shaderStages.data();
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDepthStencilState = &depthStencil;
        pipelineInfo.layout = pipelineLayout;

        VkDynamicState state[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

        VkPipelineDynamicStateCreateInfo dynamicInfo = {.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};
        dynamicInfo.pDynamicStates = &state[0];
        dynamicInfo.dynamicStateCount = 2;

        pipelineInfo.pDynamicState = &dynamicInfo;

        VkPipeline newPipeline;
        if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &newPipeline) != VK_SUCCESS) {
            IC_CORE_ERROR("Failed to create graphics pipeline.");
            throw std::runtime_error("Failed to create graphics pipeline.");
        }

        return newPipeline;
    }

    VkPipeline PipelineBuilder::BuildComputePipeline(VkDevice device) {
        VkComputePipelineCreateInfo pipelineInfo{.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO};
        pipelineInfo.pNext = nullptr;
        pipelineInfo.stage = shaderStages[0];
        pipelineInfo.layout = pipelineLayout;

        VkPipeline newPipeline;
        if (vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &newPipeline) != VK_SUCCESS) {
            IC_CORE_ERROR("Failed to create compute pipeline.");
            throw std::runtime_error("Failed to create compute pipeline.");
        }
        return newPipeline;
    }

    void PipelineBuilder::SetShaders(VkShaderModule vertexShader, VkShaderModule fragmentShader) {
        shaderStages.clear();
        shaderStages.push_back(ShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, vertexShader));
        shaderStages.push_back(ShaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentShader));
    }

    void PipelineBuilder::SetComputeShader(VkShaderModule computeShader) {
        shaderStages.push_back(ShaderStageCreateInfo(VK_SHADER_STAGE_COMPUTE_BIT, computeShader));
    }

    void PipelineBuilder::SetInputTopology(VkPrimitiveTopology topology) {
        inputAssembly.topology = topology;
        inputAssembly.primitiveRestartEnable = VK_FALSE;
    }

    void PipelineBuilder::SetPolygonMode(VkPolygonMode mode) {
        rasterizer.polygonMode = mode;
        rasterizer.lineWidth = 1.f;
    }

    void PipelineBuilder::SetCullMode(VkCullModeFlags cullMode, VkFrontFace frontFace) {
        rasterizer.cullMode = cullMode;
        rasterizer.frontFace = frontFace;
    }

    void PipelineBuilder::SetMultisamplingNone() {
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading = 1.0f;
        multisampling.pSampleMask = nullptr;
        multisampling.alphaToCoverageEnable = VK_FALSE;
        multisampling.alphaToOneEnable = VK_FALSE;
    }

    void PipelineBuilder::DisableBlending() {
        colorBlendAttachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;
    }

    void PipelineBuilder::EnableBlending() {
        colorBlendAttachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_TRUE;
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_SUBTRACT;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    }

    void PipelineBuilder::SetColorAttachmentFormat(VkFormat format) {
        colorAttachmentformat = format;
        renderInfo.colorAttachmentCount = 1;
        renderInfo.pColorAttachmentFormats = &colorAttachmentformat;
    }

    void PipelineBuilder::DisableColorAttachment() {
        renderInfo.colorAttachmentCount = 0;
        renderInfo.pColorAttachmentFormats = nullptr;
    }

    void PipelineBuilder::SetDepthFormat(VkFormat format) {
        renderInfo.depthAttachmentFormat = format;
    }

    void PipelineBuilder::DisableDepthTest() {
        depthStencil.depthTestEnable = VK_FALSE;
        depthStencil.depthWriteEnable = VK_FALSE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_NEVER;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.stencilTestEnable = VK_FALSE;
        depthStencil.front = {};
        depthStencil.back = {};
        depthStencil.minDepthBounds = 0.f;
        depthStencil.maxDepthBounds = 1.f;
    }

    void PipelineBuilder::EnableDepthTest() {
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.stencilTestEnable = VK_FALSE;
        depthStencil.front = {};
        depthStencil.back = {};
        depthStencil.minDepthBounds = 0.f;
        depthStencil.maxDepthBounds = 1.f;
    }

    VkPipelineShaderStageCreateInfo PipelineBuilder::ShaderStageCreateInfo(VkShaderStageFlagBits flags,
                                                                           VkShaderModule module) {
        VkPipelineShaderStageCreateInfo createInfo{.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
        createInfo.stage = flags;
        createInfo.module = module;
        createInfo.pName = "main";
        return createInfo;
    }

    // pipeline manager
    PipelineManager::PipelineManager(VkDevice device) : _device{device} {}

    void PipelineManager::DestroyPipelines(VkDevice device) {
        for (auto pipeline : _createdPipelines) {
            DestroyPipeline(device, *pipeline);
        }
        DestroyPipeline(device, *_shadowMapPipeline);
    }

    std::shared_ptr<Pipeline> PipelineManager::FindOrCreateSuitablePipeline(SwapChain &swapChain,
                                                                            MaterialInstance &materialData) {
        for (auto pipeline : _createdPipelines) {
            if (IsPipelineSuitable(*pipeline, materialData)) {
                return pipeline;
            }
        }
        std::shared_ptr<Pipeline> pipeline = CreateOpaquePipeline(_device, swapChain, materialData);
        _createdPipelines.push_back(pipeline);
        return pipeline;
    }

    bool PipelineManager::IsPipelineSuitable(Pipeline &pipeline, MaterialInstance &materialData) {
        // todo: oversimplification, but will do for now
        return pipeline.materialFlags == materialData.Template().flags;
    }

    /// @brief Creates the graphics pipeline for shadow render pass
    void PipelineManager::CreateShadowMapPipeline(VkFormat swapChainDepthFormat) {
        // pipeline layout
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;

        // push constants
        VkPushConstantRange pushConstants =
            PushConstants<TransformationPushConstants>(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstants;

        VkPipelineLayout pipelineLayout;
        VK_CHECK(vkCreatePipelineLayout(_device, &pipelineLayoutInfo, nullptr, &pipelineLayout));

        PipelineBuilder pipelineBuilder;
        pipelineBuilder.pipelineLayout = pipelineLayout;
        VkShaderModule vertShaderModule =
            PipelineBuilder::CreateShaderModule(_device, "src/vulkan/shaders/shadow_pass.vert.spv");
        VkShaderModule fragShaderModule =
            PipelineBuilder::CreateShaderModule(_device, "src/vulkan/shaders/shadow_pass.frag.spv");
        pipelineBuilder.SetShaders(vertShaderModule, fragShaderModule);
        pipelineBuilder.SetInputTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
        pipelineBuilder.SetPolygonMode(VK_POLYGON_MODE_FILL);
        pipelineBuilder.SetCullMode(VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_CLOCKWISE);
        pipelineBuilder.SetMultisamplingNone();
        pipelineBuilder.DisableBlending();
        pipelineBuilder.DisableColorAttachment();
        pipelineBuilder.EnableDepthTest();
        pipelineBuilder.SetDepthFormat(swapChainDepthFormat);

        _shadowMapPipeline = std::make_unique<Pipeline>();
        _shadowMapPipeline->pipeline = pipelineBuilder.BuildPipeline(_device);
        _shadowMapPipeline->layout = pipelineLayout;
        _shadowMapPipeline->descriptorSetLayouts = {};
        _shadowMapPipeline->shaderModules = {vertShaderModule, fragShaderModule};
        _shadowMapPipeline->materialFlags = MaterialFlags::None;
    }
} // namespace IC
