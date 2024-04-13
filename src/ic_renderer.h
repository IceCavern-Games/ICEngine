#pragma once

#include <ic_graphics.h>

#include <GLFW/glfw3.h>

#include <vector>

namespace IC {
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

        virtual void DrawFrame() = 0;
        virtual void AddMesh(Mesh &meshData, Material &materialData) = 0;

    protected:
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
