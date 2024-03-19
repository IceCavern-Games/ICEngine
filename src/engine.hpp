#pragma once

#include "renderer/ic_renderer.hpp"

namespace engine {
class Engine {
private:
    render::ICRenderer renderer;

public:
    Engine();
    ~Engine();

    void run();
};
} // namespace engine