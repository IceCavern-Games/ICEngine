#include "ic_renderer.h"

#include "vulkan/vulkan_renderer.h"

#include <algorithm>

namespace IC {
    Renderer::Renderer(const RendererConfig &config) : window(config.window) {
        AddImguiFunction(STATS_WINDOW_NAME, std::bind(&Renderer::RenderStatsGUI, this));
    }

    Renderer::~Renderer() {
        RemoveImguiFunction(STATS_WINDOW_NAME);
    }

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

    void Renderer::AddImguiFunction(std::string windowName, std::function<void()> function) {
        imGuiFunctions[windowName] = function;
    }

    void Renderer::RemoveImguiFunction(std::string windowName) {
        imGuiFunctions.erase(windowName);
    }
} // namespace IC
