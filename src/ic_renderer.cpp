#include <ic_renderer.h>

#include "vulkan/vulkan_renderer.h"

#include <algorithm>

namespace IC {
    Renderer::Renderer(const RendererConfig &config) : window(config.window) {}

    Renderer::~Renderer() {}

    Renderer *Renderer::MakeRenderer(const RendererConfig &rendererConfig) {
        switch (rendererConfig.rendererType) {
        case RendererType::Vulkan:
#ifdef IC_RENDERER_VULKAN
            return new VulkanRenderer(rendererConfig);
#else
            return nullptr;
#endif
        default:
            return nullptr;
        }
    }

    void Renderer::AddImguiFunction(ImGuiFunction function) {
        imGuiFunctions.push_back(function);
    }

    void Renderer::RemoveImguiFunction(ImGuiFunction function) {
        // unordered erase.
        std::erase(imGuiFunctions, function);
    }
} // namespace IC
