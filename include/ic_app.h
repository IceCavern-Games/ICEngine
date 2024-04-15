#pragma once

#include "ic_graphics.h"

namespace IC {
    // Application Configuration
    struct Config {
        // Application name.
        const char *name = "ic";

        // Which renderer to use.
#ifdef IC_RENDERER_VULKAN
        RendererType rendererType = RendererType::Vulkan;
#else
        RendererType rendererType = RendererType::None;
#endif

        // Starting width, in pixels.
        // Depending on the OS DPI, the true window size may be a multiple of this.
        int width = 1280;

        // Starting height, in pixels.
        // Depending on the OS DPI, the true window size may be a multiple of this.
        int height = 720;
    };

    // Application
    namespace App {
        // Runs the application.
        bool Run(const Config *config);

        // Checks if the Application is running yet.
        bool IsRunning();

        // Exits the application.
        void Exit();

        // Gets the config data used to run the application.
        const Config &GetConfig();
    } // namespace App
} // namespace IC
