#include "glfw_window.h"

#include <ic_common.h>
#include <ic_log.h>

#include "ic_renderer.h"

namespace IC {
    static uint8_t _glfwWindowCount = 0;

    static void GLFWErrorCallback(int error, const char *description) {
        IC_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
    }

    Window *Window::Create(const WindowProps &props) {
        return new GLFWWindow(props);
    }

    GLFWWindow::GLFWWindow(const WindowProps &props) {
        Init(props);
    }

    GLFWWindow::~GLFWWindow() {
        Exit();
    }

    void GLFWWindow::Init(const WindowProps &props) {
        _data.title = props.title;
        _data.width = props.width;
        _data.height = props.height;

        IC_CORE_INFO("Creating window {0} ({1}, {2}) using GLFW", props.title, props.width, props.height);

        if (_glfwWindowCount == 0) {
            int success = glfwInit();
            IC_CORE_ASSERT(success, "Could not initialize GLFW.");

            glfwSetErrorCallback(GLFWErrorCallback);
        }

#ifdef IC_RENDERER_VULKAN
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#endif
        _window = glfwCreateWindow((int)props.width, (int)props.height, props.title.c_str(), nullptr, nullptr);
        _glfwWindowCount++;

        RendererType rendererType = RendererType::None;
#ifdef IC_RENDERER_VULKAN
        rendererType = RendererType::Vulkan;
#endif

        _renderer = Renderer::MakeRenderer(RendererConfig(_window, rendererType, props.width, props.height));
        IC_CORE_ASSERT(_renderer != nullptr, "Render module was not found.");

        // @TODO: Set GLFW callbacks
    }

    void GLFWWindow::Exit() {
        if (_renderer != nullptr) {
            delete _renderer;
            _renderer = nullptr;
        }

        glfwDestroyWindow(_window);
        _glfwWindowCount--;

        if (_glfwWindowCount == 0) {
            glfwTerminate();
        }
    }

    void GLFWWindow::OnUpdate() {
        glfwPollEvents();
        _renderer->DrawFrame();
    }
} // namespace IC
