#include "glfw_window.h"

#include <events/ic_app_event.h>
#include <events/ic_key_event.h>
#include <ic_common.h>
#include <ic_keycodes.h>
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
        glfwSetWindowUserPointer(_window, &_data);

        _glfwWindowCount++;

        RendererType rendererType = RendererType::None;
#ifdef IC_RENDERER_VULKAN
        rendererType = RendererType::Vulkan;
#endif

        _renderer = Renderer::MakeRenderer(RendererConfig(_window, rendererType, props.width, props.height));
        IC_CORE_ASSERT(_renderer != nullptr, "Render module was not found.");

        glfwSetKeyCallback(_window, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
            WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);

            switch (action) {
            case GLFW_PRESS: {
                KeyPressedEvent event((KeyCode)key, 0);
                data.eventCallback(event);
                break;
            }
            case GLFW_RELEASE: {
                KeyReleasedEvent event((KeyCode)key);
                data.eventCallback(event);
                break;
            }
            case GLFW_REPEAT: {
                KeyPressedEvent event((KeyCode)key, 1);
                data.eventCallback(event);
                break;
            }
            }
        });

        glfwSetWindowCloseCallback(_window, [](GLFWwindow *window) {
            WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);

            WindowCloseEvent event;
            data.eventCallback(event);
        });

        glfwSetWindowSizeCallback(_window, [](GLFWwindow *window, int width, int height) {
            WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
            data.width = width;
            data.height = height;

            WindowResizeEvent event(width, height);
            data.eventCallback(event);
        });
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
