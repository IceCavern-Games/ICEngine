#include "vk_renderer.hpp"

namespace render {
VkRenderer::VkRenderer(int width, int height)
    : window{width, height, "Ice Cavern Game Engine"} {}
VkRenderer::~VkRenderer() {}

void VkRenderer::init() {}

void VkRenderer::draw() { glfwPollEvents(); }
void VkRenderer::cleanup() {}
} // namespace render