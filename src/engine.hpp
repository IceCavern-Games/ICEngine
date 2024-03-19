#pragma once

#include "renderer/vk_renderer.hpp"

namespace engine {
class Engine {
private:
    static const int INIT_WIDTH = 1600;
    static const int INIT_HEIGHT = 900;

    render::VkRenderer renderer{INIT_WIDTH, INIT_HEIGHT};

public:
    Engine();
    ~Engine();

    void run();
};
} // namespace engine