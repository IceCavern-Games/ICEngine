#pragma once

#include <renderer.h>

namespace IC
{
    // Application Configuration
    struct Config
    {
        // Application name.
        const char *name = "ic";

        // Starting width, in pixels.
        // Depending on the OS DPI, the true window size may be a multiple of this.
        int width = 1280;

        // Starting height, in pixels.
        // Depending on the OS DPI, the true window size may be a multiple of this.
        int height = 720;
    };

    // Application
    namespace App
    {
        // Runs the application.
        bool run(const Config *config);

        // Checks if the Application is running yet.
        bool is_running();

        // Exits the application.
        void exit();

        // Gets the config data used to run the application.
        const Config &config();
    }
}
