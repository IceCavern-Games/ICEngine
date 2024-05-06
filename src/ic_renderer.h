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

        RendererConfig(GLFWwindow *window, RendererType rendererType, int width, int height)
            : window(window), rendererType(rendererType), width(width), height(height) {}
    };

    struct RenderStats {
        float frametime;
        uint32_t numTris;
        uint32_t drawCalls;
    };

    class Renderer {
    public:
        Renderer(const RendererConfig &config);
        virtual ~Renderer();

        static Renderer *MakeRenderer(const RendererConfig &rendererConfig);

        virtual void AddMesh(Mesh &meshData, Material &materialData) = 0;
        virtual void AddLight(std::shared_ptr<PointLight> light) = 0;
        virtual void AddDirectionalLight(std::shared_ptr<DirectionalLight> light) = 0;
        virtual void DrawFrame() = 0;

        void AddImguiFunction(std::string windowName, std::function<void()> function);
        void RemoveImguiFunction(std::string windowName);

    protected:
        std::unordered_map<std::string, std::function<void()>> imGuiFunctions;
        GLFWwindow *window;
        RenderStats renderStats{};

        void RenderStatsGUI();

    private:
        Renderer(const Renderer &) = delete;
        Renderer &operator=(const Renderer &) = delete;
        const std::string STATS_WINDOW_NAME = "render stats";
    };
} // namespace IC
