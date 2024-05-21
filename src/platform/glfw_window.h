#pragma once

#include <ic_graphics.h>
#include <ic_window.h>

#include "ic_renderer.h"

#include <GLFW/glfw3.h>

namespace IC {
    class GLFWWindow : public Window {
    public:
        GLFWWindow(const WindowProps &props);
        virtual ~GLFWWindow();

        void OnUpdate() override;

        inline unsigned int GetWidth() const override { return _data.width; }
        inline unsigned int GetHeight() const override { return _data.height; }
        inline bool ShouldClose() const override { return glfwWindowShouldClose(_window); }

        inline virtual void *GetNativeWindow() const override { return _window; }
        inline void SetEventCallback(const EventCallbackFn &callback) override { _data.eventCallback = callback; }

    private:
        virtual void Init(const WindowProps &props);
        virtual void Exit();

    private:
        GLFWwindow *_window;
        Renderer *_renderer;

        struct WindowData {
            std::string title;
            unsigned int width, height;

            EventCallbackFn eventCallback;
        };

        WindowData _data;
    };
} // namespace IC
