#pragma once

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

        void Run();

    private:
        std::unique_ptr<Window> _window;
        bool _isRunning = true;
    };
} // namespace IC
