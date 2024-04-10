#pragma once

#include <GLFW/glfw3.h>
#include <ic_graphics.h>

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
        RendererType renderer_type;
        GLFWwindow *pWindow;
        int width;
        int height;
    };

    class ICRenderer
    {
    public:
        ICRenderer(RendererConfig config) { pWindow = config.pWindow; };
        ~ICRenderer() = default;
        virtual void draw_frame() = 0;
        virtual void add_mesh(ICMesh &meshData, ICMaterial &materialData) = 0;

    protected:
        GLFWwindow *pWindow;

        static ICRenderer *make_vulkan(RendererConfig &renderer_config);

    public:
        static ICRenderer *make_renderer(RendererConfig &renderer_config)
        {
            switch (renderer_config.renderer_type)
            {
            case RendererType::Vulkan:
                return make_vulkan(renderer_config);
            default:
                return nullptr;
            }
        }
    };
}