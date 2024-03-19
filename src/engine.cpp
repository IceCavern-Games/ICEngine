#include "engine.hpp"

namespace engine {
Engine::Engine() {}

Engine::~Engine() {}

void Engine::run() {
    renderer.init();
    // main loop
    while (!renderer.window.shouldClose()) {
        renderer.draw();
    }
    renderer.cleanup();
}
} // namespace engine