#pragma once

#include "vk_window.hpp"

namespace render {
class VkRenderer {
private:
public:
    VkRenderer(int width, int height);
    ~VkRenderer();

    void init();
    void draw();
    void cleanup();

    VkWindow window;
};
} // namespace render