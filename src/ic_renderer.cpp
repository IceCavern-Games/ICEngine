#include "ic_renderer.h"

#include "vulkan/vulkan_renderer.h"

#include <algorithm>

namespace IC {
    Renderer::Renderer(const RendererConfig &config) : window(config.window) {
        AddImguiFunction(std::bind(&Renderer::RenderStatsGUI, this));
    }

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

    void Renderer::RenderStatsGUI() {
        ImGui::Begin("Render Stats");
        ImGui::Text("frametime %f ms (%f FPS)", renderStats.frametime, 1 / (renderStats.frametime / 1000));
        ImGui::Text("rendered tris: %d", renderStats.numTris);
        ImGui::Text("draw calls: %d", renderStats.drawCalls);
        ImGui::End();
    }

    void Renderer::AddImguiFunction(std::function<void()> function) {
        imGuiFunctions.push_back(function);
    }

    void Renderer::RemoveImguiFunction(std::function<void()> function) {
        // unordered erase.
        // std::erase(imGuiFunctions, function);
    }
} // namespace IC
