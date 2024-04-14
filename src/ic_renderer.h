#pragma once

#include <ic_graphics.h>

#include <GLFW/glfw3.h>

#include <algorithm>
#include <vector>

namespace IC {
    typedef void (*ImGuiFunction)(void);

    struct RendererConfig {
        RendererType rendererType;
        GLFWwindow *window;
        int width;
        int height;
    };

    class Renderer {
    public:
        Renderer(RendererConfig config) { window = config.window; };
        ~Renderer() = default;

        void AddImguiFunction(ImGuiFunction function) { imGuiFunctions.push_back(function); };
        void RemoveImguiFunction(ImGuiFunction function) {
            imGuiFunctions.erase(std::find(imGuiFunctions.begin(), imGuiFunctions.end(), function));
        }

        virtual void AddMesh(Mesh &meshData, Material &materialData) = 0;
        virtual void DrawFrame() = 0;

    protected:
        std::vector<ImGuiFunction> imGuiFunctions;
        GLFWwindow *window;

    private:
#ifdef IC_RENDERER_VULKAN
        static Renderer *MakeVulkan(RendererConfig &rendererConfig);
#else
        static Renderer *MakeVulkan(RendererConfig &rendererConfig) {
            return nullptr;
        }
#endif

    public:
        static Renderer *MakeRenderer(RendererConfig &rendererConfig) {
            switch (rendererConfig.rendererType) {
            case RendererType::Vulkan:
                return MakeVulkan(rendererConfig);
            default:
                return nullptr;
            }
        }
    };
} // namespace IC
