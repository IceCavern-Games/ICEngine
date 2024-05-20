#include <ic_app.h>

#include <ic_log.h>

namespace IC {

#define BIND_EVENT_FN(x) std::bind(&App::x, this, std::placeholders::_1)

    App::App(const Config &config) {
        Log::Init();

        _window = std::unique_ptr<Window>(Window::Create(WindowProps(config.name)));
        _window->SetEventCallback(BIND_EVENT_FN(OnEvent));

        // @TODO: Init Renderer
    }

    App::~App() {}

    void App::Run() {
        while (_isRunning) {
            _window->OnUpdate();
        }
    }

    void App::OnEvent(Event &e) {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
        dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));
    }

    bool App::OnWindowClose(WindowCloseEvent &e) {
        _isRunning = false;

        return true;
    }

    bool App::OnWindowResize(WindowResizeEvent &e) {
        IC_CORE_TRACE("Window Resized: {0}, {1}", e.GetWidth(), e.GetHeight());

        // @TODO: Renderer resize

        return false;
    }
} // namespace IC
