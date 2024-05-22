#pragma once

#include <ic_gameobject.h>
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

        virtual void AddGameObject(std::shared_ptr<GameObject> object) = 0;
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
