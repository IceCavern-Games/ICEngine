#pragma once

#include "ic_device.hpp"
#include "ic_swap_chain.hpp"
#include "ic_window.hpp"
#include "mesh.hpp"

#include <vector>

namespace render {
class ICRenderer {
private:
    ICWindow icWindow{1600, 900, "ICEngine"};
    ICDevice icDevice{icWindow};
    ICSwapChain icSwapChain{icDevice, icWindow.getExtent()};

    Pipeline testPipeline{};
    Mesh triangle{icDevice};
    std::vector<VkCommandBuffer> commandBuffers;

public:
    ICRenderer();
    ~ICRenderer();

    bool shouldClose() { return icWindow.shouldClose(); }

    void init();
    void cleanup();
    void createCommandBuffers();
    void draw();
};
} // namespace render