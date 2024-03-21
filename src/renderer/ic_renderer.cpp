#include "ic_renderer.hpp"
#include "util/images.hpp"
#include "util/initializers.hpp"
#include "util/pipeline_builder.hpp"

namespace render {
ICRenderer::ICRenderer() {}
ICRenderer::~ICRenderer() {}

void ICRenderer::init() {
    createCommandBuffers();

    // build pipeline for simple triangle
    // pipeline layout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;

    VkPipelineLayout pipelineLayout;
    VK_CHECK(
        vkCreatePipelineLayout(icDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout));

    VkShaderModule vertShaderModule = util::PipelineBuilder::createShaderModule(
        icDevice.device(), "src/shaders/simple_shader.vert.spv");
    VkShaderModule fragShaderModule = util::PipelineBuilder::createShaderModule(
        icDevice.device(), "src/shaders/simple_shader.frag.spv");

    util::PipelineBuilder builder;
    builder.pipelineLayout = pipelineLayout;
    builder.setShaders(vertShaderModule, fragShaderModule);
    builder.setInputTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    builder.setPolygonMode(VK_POLYGON_MODE_FILL);
    builder.setCullMode(VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE);
    builder.setMultisamplingNone();
    builder.disableBlending();
    builder.enableDepthTest();
    builder.setColorAttachmentFormat(icSwapChain.getSwapChainImageFormat());
    builder.setDepthFormat(icSwapChain.getSwapChainDepthFormat());

    testPipeline.pipeline = builder.buildGraphicsPipeline(icDevice.device());
    testPipeline.layout = pipelineLayout;
    testPipeline.descriptorSetLayout = nullptr;
    testPipeline.shaderModules = {vertShaderModule, fragShaderModule};
    testPipeline.transparent = false;
}

void ICRenderer::draw() {
    glfwPollEvents();

    // begin render
    uint32_t imageIndex;
    auto result = icSwapChain.acquireNextImage(&imageIndex);

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image");
    }

    icSwapChain.waitForFrameFence(&imageIndex);

    VK_CHECK(vkResetCommandBuffer(commandBuffers[imageIndex], 0));

    VkCommandBufferBeginInfo beginInfo = init::commandBufferBeginInfo();

    if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer");
    }

    VkRenderingAttachmentInfo colorAttachment = {.sType =
                                                     VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO};
    colorAttachment.imageView = icSwapChain.getImageView(imageIndex);
    colorAttachment.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    colorAttachment.clearValue = {0.1f, 0.1f, 0.1f, 1.0f};
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    VkRenderingAttachmentInfo depthAttachment{.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO};
    depthAttachment.imageView = icSwapChain.getDepthImageView(imageIndex);
    depthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
    depthAttachment.clearValue.depthStencil = {1.0f, 0};
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    VkRenderingInfo renderingInfo{.sType = VK_STRUCTURE_TYPE_RENDERING_INFO};
    renderingInfo.renderArea = VkRect2D{VkOffset2D{0, 0}, icSwapChain.getSwapChainExtent()};
    renderingInfo.layerCount = 1;
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachments = &colorAttachment;
    renderingInfo.pDepthAttachment = &depthAttachment;
    renderingInfo.pStencilAttachment = nullptr;
    vkCmdBeginRendering(commandBuffers[imageIndex], &renderingInfo);

    VkViewport viewport = {};
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = icSwapChain.getSwapChainExtent().width;
    viewport.height = icSwapChain.getSwapChainExtent().height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);

    VkRect2D scissor = {};
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent.width = icSwapChain.getSwapChainExtent().width;
    scissor.extent.height = icSwapChain.getSwapChainExtent().height;

    vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);
    vkCmdBindPipeline(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS,
                      testPipeline.pipeline);

    util::transitionImageLayout(commandBuffers[imageIndex], icSwapChain.getImage(imageIndex),
                                icSwapChain.getSwapChainImageFormat(), VK_IMAGE_LAYOUT_UNDEFINED,
                                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    triangle.bind(commandBuffers[imageIndex], testPipeline.layout, icSwapChain.getCurrentFrame());
    triangle.draw(commandBuffers[imageIndex]);
    vkCmdEndRendering(commandBuffers[imageIndex]);

    util::transitionImageLayout(commandBuffers[imageIndex], icSwapChain.getImage(imageIndex),
                                icSwapChain.getSwapChainImageFormat(),
                                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    VK_CHECK(vkEndCommandBuffer(commandBuffers[imageIndex]));

    VK_CHECK(icSwapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex));
}

void ICRenderer::cleanup() {
    vkDeviceWaitIdle(icDevice.device());
    util::PipelineBuilder::destroyPipeline(icDevice.device(), testPipeline);
}

void ICRenderer::createCommandBuffers() {
    commandBuffers.resize(icSwapChain.imageCount());
    VkCommandBufferAllocateInfo allocInfo = init::commandBufferAllocateInfo(
        icDevice.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()));
    VK_CHECK(vkAllocateCommandBuffers(icDevice.device(), &allocInfo, commandBuffers.data()));
}
} // namespace render