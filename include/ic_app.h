#pragma once

#include "ic_renderer.h"

namespace IC
{
    // Application Configuration
    struct Config
    {
        // Application name.
        const char *Name = "ic";

        // Starting width, in pixels.
        // Depending on the OS DPI, the true window size may be a multiple of this.
        int Width = 1280;

        // Starting height, in pixels.
        // Depending on the OS DPI, the true window size may be a multiple of this.
        int Height = 720;
    };

    // Application
    namespace App
    {
        // Runs the application.
        bool Run(const Config *config);

        // Checks if the Application is running yet.
        bool IsRunning();

        // Exits the application.
        void Exit();

        // Gets the config data used to run the application.
        const Config &GetConfig();
    }
}
