#pragma once

#include "ic_window.hpp"

namespace render {
class ICRenderer {
private:
public:
    ICRenderer(int width, int height);
    ~ICRenderer();

    void init();
    void draw();
    void cleanup();

    ICWindow window;
};
} // namespace render