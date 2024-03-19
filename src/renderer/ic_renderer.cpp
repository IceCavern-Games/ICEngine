#include "ic_renderer.hpp"

namespace render {
ICRenderer::ICRenderer() {}
ICRenderer::~ICRenderer() {}

void ICRenderer::init() {}

void ICRenderer::draw() { glfwPollEvents(); }
void ICRenderer::cleanup() {}
} // namespace render