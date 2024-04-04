#pragma once

namespace IC::Renderer
{
    enum class RendererType
    {
        None = -1,
        Vulkan
    };

    class Renderer 
    {
    public:
        virtual void draw() = 0;
        virtual void draw_mesh() = 0;

    private:
        static Renderer* make_vulkan();
    public:
        static Renderer* make_renderer(RendererType type) 
        {
            switch(type)
            {
                case RendererType::None: return nullptr;
                case RendererType::Vulkan: return make_vulkan();
            }
        }
    };
}