#pragma once

#include <functional>
#include <string>

namespace IC {
    struct WindowProps {
        std::string title;
        unsigned int width;
        unsigned int height;

        WindowProps(const std::string &title = "ic", unsigned int width = 1280, unsigned int height = 720)
            : title(title), width(width), height(height) {}
    };

    class Window {
    public:
        virtual ~Window() {}

        virtual void OnUpdate() = 0;

        virtual unsigned int GetWidth() const = 0;
        virtual unsigned int GetHeight() const = 0;
        virtual bool ShouldClose() const = 0;

        virtual void *GetNativeWindow() const = 0;

        static Window *Create(const WindowProps &props = WindowProps());
    };
} // namespace IC
