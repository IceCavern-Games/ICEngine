#include "ic_renderer.hpp"

namespace render {
ICRenderer::ICRenderer(int width, int height) : window{width, height, "ICEngine"} {}
ICRenderer::~ICRenderer() {}

void ICRenderer::init() {}

void ICRenderer::draw() { glfwPollEvents(); }
void ICRenderer::cleanup() {}
} // namespace render