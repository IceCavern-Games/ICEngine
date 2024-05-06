#include <ic_app.h>

#include <ic_log.h>

namespace IC {
    App::App(const Config &config) {
        Log::Init();
        _window = std::unique_ptr<Window>(Window::Create(WindowProps(config.name)));
    }

    App::~App() {}

    void App::Run() {
        // @TODO: Switch to _isRunning
        while (!_window->ShouldClose()) {
            _window->OnUpdate();
        }
    }
} // namespace IC
