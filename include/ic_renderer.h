#pragma once

#include <GLFW/glfw3.h>
#include "ic_graphics.h"

#include <vector>

namespace IC::Renderer
{
    enum class RendererType
    {
        None = -1,
        Vulkan
    };

    struct RendererConfig
    {
        RendererType RendererType;
        GLFWwindow *Window;
        int Width;
        int Height;
    };

    class Renderer
    {
    protected:
        GLFWwindow *Window;

        static Renderer *MakeVulkan(RendererConfig &rendererConfig);

    public:
        Renderer(RendererConfig config) { Window = config.Window; };
        ~Renderer() = default;

        virtual void DrawFrame() = 0;
        virtual void AddMesh(Mesh &meshData, Material &materialData) = 0;

        static Renderer *MakeRenderer(RendererConfig &rendererConfig)
        {
            switch (rendererConfig.RendererType)
            {
            case RendererType::Vulkan:
                return MakeVulkan(rendererConfig);
            default:
                return nullptr;
            }
        }
    };
}
