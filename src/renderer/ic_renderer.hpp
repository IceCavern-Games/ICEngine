#pragma once

#include "ic_device.hpp"
#include "ic_swap_chain.hpp"
#include "ic_window.hpp"

namespace render {
class ICRenderer {
private:
    ICWindow icWindow{1600, 900, "ICEngine"};
    ICDevice icDevice{icWindow};
    ICSwapChain icSwapChain{icDevice, icWindow.getExtent()};

public:
    ICRenderer();
    ~ICRenderer();

    bool shouldClose() { return icWindow.shouldClose(); }

    void init();
    void draw();
    void cleanup();
};
} // namespace render