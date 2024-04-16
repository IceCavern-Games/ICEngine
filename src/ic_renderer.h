#pragma once

#include <ic_graphics.h>

#include <GLFW/glfw3.h>

#include <algorithm>
#include <functional>
#include <memory>
#include <vector>

struct GLFWwindow;

namespace IC {

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
        virtual void AddLight(std::shared_ptr<PointLight> light) = 0;
        virtual void DrawFrame() = 0;

        void AddImguiFunction(std::function<void()> function);
        void RemoveImguiFunction(std::function<void()> function);

    protected:
        std::vector<std::function<void()>> imGuiFunctions;
        GLFWwindow *window;

    private:
        Renderer(const Renderer &) = delete;
        Renderer &operator=(const Renderer &) = delete;
    };
} // namespace IC
