#pragma once

#include "events/ic_app_event.h"
#include "events/ic_event.h"
#include "ic_window.h"

#include <string>

namespace IC {
    struct Config {
        std::string name = "ic";
    };

    class App {
    public:
        App(const Config &config);
        virtual ~App();

        void OnEvent(Event &e);

        void Run();

    private:
        std::unique_ptr<Window> _window;
        bool _isRunning = true;

        bool OnWindowClose(WindowCloseEvent &e);
        bool OnWindowResize(WindowResizeEvent &e);
    };
} // namespace IC
