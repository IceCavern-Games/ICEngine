#pragma once

#include <ic_graphics.h>

#include <vector>

struct GLFWwindow;

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
        Renderer(const RendererConfig &config);
        virtual ~Renderer();

        static Renderer *MakeRenderer(const RendererConfig &rendererConfig);

        virtual void AddMesh(Mesh &meshData, Material &materialData) = 0;
        virtual void DrawFrame() = 0;

        void AddImguiFunction(ImGuiFunction function);
        void RemoveImguiFunction(ImGuiFunction function);

    protected:
        std::vector<ImGuiFunction> imGuiFunctions;
        GLFWwindow *window;

    private:
        Renderer(const Renderer &) = delete;
        Renderer &operator=(const Renderer &) = delete;
    };
} // namespace IC
