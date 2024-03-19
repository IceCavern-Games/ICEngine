#pragma once

#include "ic_types.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace render {
class ICWindow {
private:
    int width, height;
    std::string windowName;
    GLFWwindow *pWindow;

    void initWindow();

public:
    ICWindow(int w, int h, std::string name);
    ~ICWindow();

    ICWindow(const ICWindow &) = delete;
    ICWindow &operator=(const ICWindow &) = delete;

    bool shouldClose() { return glfwWindowShouldClose(pWindow); }
    VkExtent2D getExtent() {
        return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
    }
    GLFWwindow *getWindow() { return pWindow; }

    void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);
};
} // namespace render