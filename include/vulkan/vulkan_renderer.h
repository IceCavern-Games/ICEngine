#pragma once;
#include <renderer.h>

namespace IC::Renderer
{
    class VulkanRenderer : Renderer
    {
    public:
        void draw();
        void draw_mesh();

    private:
        static Renderer* make_vulkan();
    };
}